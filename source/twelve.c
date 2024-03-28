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
 
  //this Code is dirived from Ralhs atari/neogeo work, Below is his Preamble 

			/* Nes/Snes/Genesis/SMS/Atari to USB
			 * Copyright (C) 2006-2007 Raphaël Assénat
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
			 * The author may be contacted at raph@raphnet.net
			 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "gamepad.h"
static void twelveInit(void);
static void twelveUpdate(void);
static unsigned char twelveProbe(void);
Gamepad *twelveGetGamepad(void);
static char padType = 0;
static void twelveInit(void)
{
if (padType==2) {setDB9Mode_sega();} 
else setDB9Mode_atari_normal();
DDRC=0;
DDRB=0;
DDRD &= ~0x08;
PORTC=0xff;
PORTB=0xff;
}
static void twelveUpdate (void)
{
unsigned char data[2];
setDB9Mode_sega();
_delay_us(10);
data[0] = PINC;
data[1] = PINB;
data[0] = data[0] ^ 0xff;
data[1] = data[1] ^ 0xff;
if (data[0] & 0x20) { reportBuffer[1] = 0; } 
if (data[0] & 0x10) { reportBuffer[1] = 255; } 
if (data[0] & 0x08) { reportBuffer[0] = 0; }  
if (data[0] & 0x04) { reportBuffer[0] = 255; } 
if (padType==1)
{
if (~PINC & 0x02) reportBuffer[6] |= 0x01;
PORTD &= ~0x08; PORTD |= 0x10; 
_delay_us(10);
switch(PINC)
{
case 0x3D: reportBuffer[6] |= 0x02; break;
case 0x3B: reportBuffer[6] |= 0x04; break;
case 0x37: reportBuffer[6] |= 0x08;break;
case 0x1B:reportBuffer[6] |= 0x10;break;
case 0x7: reportBuffer[6] |= 0x20;break;
case 0x27: reportBuffer[6] |= 0x40;break;
case 0x33: reportBuffer[7] |= 0x01;break;
case 0x23:reportBuffer[7] |= 0x02;break;
case 0x2F: reportBuffer[7] |= 0x04;break;
case 0x2B: reportBuffer[7] |= 0x08;break;
case 0xF: reportBuffer[7] |= 0x10;break;
case 0x17: reportBuffer[7] |= 0x20;break;
}
}
else 
{
if (data[0] & 0x02) reportBuffer[6] |= 0x01;
if (data[1] & 0x20) reportBuffer[6] |= 0x02;
if (data[0] & 0x01) reportBuffer[6] |= 0x04;
if (data[1] & 0x10) reportBuffer[6] |= 0x08;
if (data[1] & 0x08) reportBuffer[6] |= 0x10;
if (data[1] & 0x04) reportBuffer[6] |= 0x20;
}
if (data[1] & 0x01) reportBuffer[6] |= 0x40;
if (data[1] & 0x02) reportBuffer[6] |= 0x80;
}	
static unsigned  char twelveProbe(void)
{
setDB9Mode_sega();
DDRC = 0xff;
PORTC=0;  asm("nop\n ");
DDRC = 0;
_delay_us(10);
if ( PINC == 0x34 ) padType = 1;
DDRB  |=  (1<<0);
PORTB &= ~(1<<0);
asm("nop\n ");
if (! (PINB & (1<<4)) ) padType = 2;
return 1;	
}
Gamepad twelveGamepad = {
.init 		 =  twelveInit,
.update 	 =	twelveUpdate,
.probe		 =  twelveProbe
};
Gamepad *twelveGetGamepad(void)
{
return &twelveGamepad;
}
