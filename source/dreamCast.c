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
 #include <util/delay.h>
#include "gamepad.h"
#define WAIT5 asm("nop\n nop\n nop\n nop\n nop\n");
#define WAIT4 asm("nop\n nop\n nop\n nop\n");
#define WAIT3 asm("nop\n nop\n nop\n");
#define WAIT2 asm("nop\n nop\n");
#define WAIT1 asm("nop\n ");
#define __SREG__ _SFR_IO_ADDR(SREG)
static void dreamCastInit(void);
static void dreamCastUpdate(void);
static char dreamCastProbe(void);
static unsigned char dataReady = 1;
Gamepad *dreamCastGetGamepad(void);
static unsigned char timer =0,DC_Found,ASCI = 0;
static unsigned char getBit( unsigned char in, unsigned char bit ){in = in >> bit;return !(in & 1 );}
static char oscilator = 0x2f;
static void sendData(unsigned char data)
{	
for ( char loop=7;loop>=0;loop--)
{	
if ( (data >> loop) & 1 ) 
{
PORTC = oscilator; 
PORTC = 0x3f;	  
PORTC = ~oscilator;
}
else
{
PORTC = oscilator;
PORTC = 0;
}	
oscilator = ~oscilator;
}
}
static void sendStart(void)
{
DDRC = 0x3F;
PORTC= 0x3F;
WAIT1
PORTC = 0x1f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x1f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x1f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x1f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x1f;WAIT2;
PORTC = 0x3F;
}
static void sendEnd(void)
{
PORTC = 0x3f;WAIT1;
PORTC = 0x2f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x2f;WAIT2;
PORTC = 0;	 WAIT2;
PORTC = 0x2f;WAIT2;
PORTC = 0x3f;
}
static void getRequest(void)
{
DDRC = 0x3F;
PORTC= 0x3F;
sendStart();	
sendData(0b100000001); 
sendData(0b100000000);
sendData(0b100100000);
sendData(0b100001001);
sendData(0b100000001);
sendData(0b100000000);
sendData(0b100000000);
sendData(0b100000000);
sendData(0b100101001);
sendEnd();
}
static void deviceRequest(void)
{
DDRC = 0x3F;
PORTC= 0x3F;
sendStart();
sendData(0b100000000);
sendData(0b100000000);
sendData(0b100100000);
sendData(0b100000001);
sendData(0b100100001);
sendEnd();
}

static void dreamCastInit(void)
{
timer =0;
deviceRequest();
DDRC = 0;
_delay_us(1);
if ( PINC != 0x3f) 
{
DC_Found=0;
return;
}
DC_Found=1;
while ( PINC == 0x3f && timer < 100 )
{
timer++;
WAIT1
}    
if (  timer > 96 ) DC_Found=0;
DDRC = 0xff;
timer=0;
}


static void dreamCastUpdate(void)
{
PCICR =0;
PCMSK1=0;
sega_buffer[3]=0;
getRequest();
DDRC = 0;
PCICR = (1<<PCIE1);
PCMSK1 = (1<<PCINT12);
_delay_us(69);
PCICR =0;
PCMSK1=0;
if ( sega_buffer[3]==0)
{
reportBuffer[6] |= 0x10;
PCICR =0;
PCMSK1=0;
deviceRequest();
DDRC = 0;
return;
} 
DDRC = 0xff;
reportBuffer[6] = reportBuffer[7] =0;
reportBuffer[0] = reportBuffer[1] =127;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 5) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 6) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 7) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 3) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[3], 1) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[3], 2) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 1) ;
reportBuffer[6] <<= 1;	reportBuffer[6] |= getBit( sega_buffer[4], 2) ;
reportBuffer[7] <<= 1;	reportBuffer[7] |= getBit( sega_buffer[3], 3) ;
reportBuffer[7] <<= 1;	reportBuffer[7] |= getBit( sega_buffer[3], 4) ;
reportBuffer[7] <<= 1;	reportBuffer[7] |= getBit( sega_buffer[4], 0) ;
reportBuffer[7] <<= 1;	reportBuffer[7] |= getBit( sega_buffer[3], 0) ;
reportBuffer[7] <<= 1;	reportBuffer[7] |= getBit( sega_buffer[4], 4) ;
reportBuffer[5] =  sega_buffer[1]; 
reportBuffer[4] =  sega_buffer[2]; 
reportBuffer[3] =  0; 
reportBuffer[2] =  0; 
if (ASCI == 1)
{
if ( getBit( sega_buffer[4], 6) )reportBuffer[0]=0;
if ( getBit( sega_buffer[4], 7) )reportBuffer[0]=255;
if ( getBit( sega_buffer[4], 5) )reportBuffer[1]=255;
if ( getBit( sega_buffer[4], 4) )reportBuffer[1]=0;
}
else
{
reportBuffer[1]=sega_buffer[7];
reportBuffer[0]=sega_buffer[8];
}
}
static char dreamCastProbe(void)
{
return DC_Found;
}
Gamepad DREAMCAST_Gamepad = {
.init					= dreamCastInit,
.update					= dreamCastUpdate,
.probe					= dreamCastProbe,
};
Gamepad *dreamCastGetGamepad(void)
{
return &DREAMCAST_Gamepad;
}



