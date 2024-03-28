/* Bliss-Box soruce code..
 * Copyright (C) 2007-2010 Sean Green
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The author may be contacted at S_K_U_N_X@yahoo.com
 */

 //This is the main entry to Version 1.0 of Bliss-Box. Appoligies for the lack of commants.
 // Code was was assembled for the release of version 1.0 and compliance of GPL. I will 
 // be releasing 2.0 when development is complete. 
#define DEBUG_LEVEL 0
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "usbdrv.h"
#include "gamepad.h"
static uchar *rt_usbHidReportDescriptor=NULL;
static uchar rt_usbHidReportDescriptorSize=0;
static uchar *rt_usbDeviceDescriptor=NULL;
static uchar rt_usbDeviceDescriptorSize=0;
static uchar *rt_usbDeviceName=NULL;
static uchar rt_usbDeviceNameSize=0;
static Gamepad *curGamepad;
PROGMEM int usbDescriptorStringSerialNumber[]  = {
 	USB_STRING_DESCRIPTOR_HEADER(4),
	'1','0','0','0'
};
char usbDescriptorConfiguration[] = { 0 }; // dummy
uchar my_usbDescriptorConfiguration[] = {    /* USB configuration descriptor */
    9,          /* sizeof(usbDescriptorConfiguration): length of descriptor in bytes */
    USBDESCR_CONFIG,    /* descriptor type */
    18 + 7 * USB_CFG_HAVE_INTRIN_ENDPOINT + 9, 0,
                /* total length of data returned (including inlined descriptors) */
    1,          /* number of interfaces in this configuration */
    1,          /* index of this configuration */
    0,          /* configuration name string index */
    USB_CFG_IS_SELF_POWERED,  /* attributes */

    USB_CFG_MAX_BUS_POWER/2,            /* max USB current in 2mA units */
/* interface descriptor follows inline: */
    9,          /* sizeof(usbDescrInterface): length of descriptor in bytes */
    USBDESCR_INTERFACE, /* descriptor type */
    0,          /* index of this interface */
    0,          /* alternate setting for this interface */
    USB_CFG_HAVE_INTRIN_ENDPOINT,   /* endpoints excl 0: number of endpoint descriptors to follow */
    USB_CFG_INTERFACE_CLASS,
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    0,          /* string index for interface */
//#if (USB_CFG_DESCR_PROPS_HID & 0xff)    /* HID descriptor */
    9,          /* sizeof(usbDescrHID): length of descriptor in bytes */
    USBDESCR_HID,   /* descriptor type: HID */
    0x01, 0x01, /* BCD representation of HID version */
    0x00,       /* target country code */
    0x01,       /* number of HID Report (or other HID class) Descriptor infos to follow */
    0x22,       /* descriptor type: report */
    USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH, 0,  /* total length of report descriptor */
//#endif
#if USB_CFG_HAVE_INTRIN_ENDPOINT    /* endpoint descriptor for endpoint 1 */
    7,          /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,  /* descriptor type = endpoint */
    0x81,       /* IN endpoint number 1 */
    0x03,       /* attrib: Interrupt endpoint */
    8, 0,       /* maximum packet size */
    USB_CFG_INTR_POLL_INTERVAL, /* in ms */
#endif
};
/* ----------------------- hardware I/O abstraction ------------------------ */
static void usbReset(void)
{
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	uchar i = 0;
    while(--i){             	// USB disconnect for >250ms
        _delay_ms(1);
    };
    usbDeviceConnect();
}
/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */
static uchar    idleRate;           /* in 4 ms units */
uchar	usbFunctionDescriptor(struct usbRequest *rq)
{
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_STANDARD)
		return 0;

	if (rq->bRequest == USBRQ_GET_DESCRIPTOR)
	{
		// USB spec 9.4.3, high byte is descriptor type
		switch (rq->wValue.bytes[1])
		{
			case USBDESCR_DEVICE:
				usbMsgPtr = rt_usbDeviceDescriptor;		
				return rt_usbDeviceDescriptorSize;
			case USBDESCR_HID_REPORT:
				usbMsgPtr = rt_usbHidReportDescriptor;
				return rt_usbHidReportDescriptorSize;
			case USBDESCR_CONFIG:
				usbMsgPtr = my_usbDescriptorConfiguration;
				return sizeof(my_usbDescriptorConfiguration);
		}
	}

	return 0;
}

uchar usbFunctionSetup(uchar data[8])
{
	return 0;
}

/* ------------------------------------------------------------------------- */
static void detect()
{
setDB9Mode_sega();
curGamepad = snesGetGamepad();curGamepad->init();
if ( curGamepad->probe() > 0) 	return;	curGamepad = dreamCastGetGamepad();curGamepad->init();
if ( curGamepad->probe() ) 		return;	curGamepad = psxGetGamepad();  curGamepad->init();
SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(1<<DORD)|(1<<CPHA)|(1<<CPOL);
if (curGamepad->probe() ) 		return;
SPCR=0;	curGamepad = saturnGetGamepad();curGamepad->init();
if ( curGamepad->probe() )return;curGamepad = db9GetGamepad();   curGamepad->init();	
if ( curGamepad->probe() ) 		return;curGamepad = nGetGamepad(); 	curGamepad->init();
if ( curGamepad->probe() )   return;curGamepad = jaguarGetGamepad();curGamepad->init();
if ( curGamepad->probe() ) 	return;	  
else curGamepad = twelveGetGamepad();curGamepad->init();curGamepad->probe();	
}

int main(void)
{

	unsigned char sreg;
	sreg = SREG;
	cli();
	_delay_ms(99);

	detect();

	rt_usbHidReportDescriptor = (void*)analog_usbHidReportDescriptor;
	rt_usbHidReportDescriptorSize = getAnalogUsbHidReportDescriptor_size();
	rt_usbDeviceDescriptor = (void*)usbDescrDevice;
	rt_usbDeviceDescriptorSize = getUsbDescrDevice_size();
	my_usbDescriptorConfiguration[25] = rt_usbHidReportDescriptorSize;

	usbInit();
	usbReset();
	curGamepad->init();
	SREG = sreg;

	sei();


	for(;;)
	{
		reportBuffer[0]=128;
		reportBuffer[1]=128;
		reportBuffer[2]=reportBuffer[3]=128;
		reportBuffer[4]=reportBuffer[5]=128;
		reportBuffer[6]=reportBuffer[7]=reportBuffer[8]=0;
		curGamepad->update();
		while (!usbInterruptIsReady()) usbPoll();
		usbSetInterrupt((void *)&reportBuffer + 0,8);
		while (!usbInterruptIsReady()) usbPoll();
		usbSetInterrupt((void *)&reportBuffer + 8, 1);
	}
	return 0;
}

