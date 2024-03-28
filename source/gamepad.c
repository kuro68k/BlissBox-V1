#include "gamepad.h"
#include "usbdrv.h"
#include "usbconfig.h"

#define USBDESCR_DEVICE  1

const char usbDescrDevice[] PROGMEM = {    /* USB device descriptor */
    18,         /* sizeof(usbDescrDevice): length of descriptor in bytes */
    USBDESCR_DEVICE,    /* descriptor type */
    0x01, 0x01, /* USB version supported */
    USB_CFG_DEVICE_CLASS,
    USB_CFG_DEVICE_SUBCLASS,
    0,          /* protocol */
    8,          /* max packet size */
    USB_CFG_VENDOR_ID,  /* 2 bytes */
    USB_CFG_DEVICE_ID,  /* 2 bytes */
    USB_CFG_DEVICE_VERSION, /* 2 bytes */

#if USB_CFG_VENDOR_NAME_LEN
    1,          /* manufacturer string index */
#else
    0,          /* manufacturer string index */
#endif
#if USB_CFG_DEVICE_NAME_LEN
    2,          /* product string index */
#else
    0,          /* product string index */
#endif
#if USB_CFG_SERIAL_NUMBER_LENGTH
    3,          /* serial number string index */
#else
    0,          /* serial number string index */
#endif
    1,          /* number of configurations */
};

int getUsbDescrDevice_size(void) { return sizeof(usbDescrDevice); }


const char analog_usbHidReportDescriptor[] PROGMEM = {
0x05, 0x01,0x09, 0x05,0xa1, 0x01,0x09, 0x01,0xa1, 0x00,
0x05, 0x01,0x09, 0x30,  0x09, 0x31,0x09, 0x33,0x09, 0x34,0x09, 0x35,	
0x09, 0x36,	  0x15, 0x00, 0x26, 0xFF, 0x00,0x75, 0x08,0x95, 0x06,0x81, 0x02,
0xc0, 0x05, 0x09, 0x19, 0x01,  0x29, 0x18, 0x15, 0x00,0x25, 0x01, 0x75, 0x01,
0x95, 0x18, 0x81, 0x02, 0xc0
};


int getAnalogUsbHidReportDescriptor_size(void)
{
return sizeof(analog_usbHidReportDescriptor);
}
