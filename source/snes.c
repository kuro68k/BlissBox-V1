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
 
  //this Code is dirived from Ralhs (s)Nes work Below is his Preamble 

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
#define LATCH_DDR	DDRC
#define LATCH_PORT	PORTC
#define CLOCK_DDR	DDRC
#define CLOCK_PORT	PORTC
static unsigned char  DATA_PORT, DATA_DDR, CLOCK_BIT, LATCH_BIT, DATA_BIT, DATA_PIN;
#define LATCH_LOW()	do { LATCH_PORT &= ~(LATCH_BIT); } while(0)
#define LATCH_HIGH()	do { LATCH_PORT |= LATCH_BIT; } while(0)
#define CLOCK_LOW()	do { CLOCK_PORT &= ~(CLOCK_BIT); } while(0)
#define CLOCK_HIGH()	do { CLOCK_PORT |= CLOCK_BIT; } while(0)
#define delayClock(clock,count) if(clock) {CLOCK_HIGH();_delay_us(count);} else {CLOCK_LOW(); _delay_us(count);}
static void snesInit(void);
static void snesUpdate(void);
inline short GET_DATA (void) __attribute__((always_inline));
Gamepad *SNES_GetGamepad(void);
static unsigned char gamePadType=0;
static unsigned char naki=0;
inline short  GET_DATA(void)
{
if(gamePadType==1) return PINB & DATA_BIT;
else if(gamePadType==2 || gamePadType==3 || gamePadType==4) return PINC & DATA_BIT;
}
Gamepad *snesGetGamepad(void);
static void snesInit(void)
{
setDB9Mode_sega();
DDRC |= (1<<2);  
PORTC &= ~(1<<2);
if (gamePadType==0) return;
setUpPins();
}
void setUpPins()
{
LATCH_DDR |= LATCH_BIT;
CLOCK_DDR |= CLOCK_BIT;
DATA_DDR &= ~(DATA_BIT);
DATA_PORT |= DATA_BIT;
CLOCK_PORT |= CLOCK_BIT;
LATCH_PORT &= ~(LATCH_BIT);	
}
static void snesUpdate(void)
{
unsigned char tmp=0; 
unsigned char pause=0;
if ( gamePadType == 1 )
{
delayClock ( 0, 2);delayClock ( 1, 2);
delayClock ( 0, 2);delayClock ( 1, 2);
delayClock ( 0, 2);pause =  GET_DATA();delayClock ( 1, 2);
if(naki==1)
{
delayClock ( 0, 2);pause =  GET_DATA();delayClock ( 1, 2);
delayClock ( 0, 2);delayClock ( 1, 2);
}
reportBuffer[0]=128; reportBuffer[1]=128;
delayClock (0, 2); if ( GET_DATA() ) { reportBuffer[1]=255; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { reportBuffer[1]=0; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { reportBuffer[0]=255; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { reportBuffer[0]=0; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x01; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x02; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x04; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x08; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x10; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x20; } delayClock (1, 2);
delayClock (0, 2); if ( GET_DATA() ) { tmp |= 0x40; } delayClock (1, 2);
if (pause)
{
reportBuffer[0]=128; 
reportBuffer[1]=128; 
reportBuffer[6] = 0x10;
}
else reportBuffer[6] = tmp;
}
else if ( gamePadType == 2 || gamePadType == 4 )
{
uint8_t i;
LATCH_HIGH();LATCH_LOW();
for (i=0; i<8; i++)
{
CLOCK_LOW();
tmp <<= 1;	
if (!GET_DATA()) { tmp |= 1; }
CLOCK_HIGH();
}
if ( tmp  == 0xff )  
{ 
reportBuffer[0]=reportBuffer[1]=128;
reportBuffer[6]=0x08; return; 
}
reportBuffer[0] = tmp;
for (i=0; i<8; i++)
{
CLOCK_LOW();
tmp >>= 1;	
if (!GET_DATA()) { tmp |= 0x80; }
CLOCK_HIGH();
}
reportBuffer[1] = tmp;
unsigned char x, y;
unsigned char lrcb1, lrcb2;
unsigned char nes_mode = 0;
lrcb1 = reportBuffer[0];
lrcb2 = reportBuffer[1];
reportBuffer[6] = lrcb2;
if ( (lrcb2 & 0xf0) == 0xf0) {
nes_mode = 1;
} else {
nes_mode = 0;
}
y = x = 128;
if (lrcb1&0x1) { x = 255; }
if (lrcb1&0x2) { x = 0; }
if (lrcb1&0x4) { y = 255; }
if (lrcb1&0x8) { y = 0; }
reportBuffer[0]=x;
reportBuffer[1]=y;
reportBuffer[6] =	(lrcb1&0x80)>>7;
reportBuffer[6] |=	(lrcb1&0x40)>>5;
reportBuffer[6] |=	(lrcb1&0x20)>>3;
reportBuffer[6] |=	(lrcb1&0x10)>>1;
if (!nes_mode)
{	
reportBuffer[6] |=	(lrcb2&0x0f)<<4;	
}
if (gamePadType == 4)
{
reportBuffer[7] = ( lrcb2 & 0xc) >> 2; 
reportBuffer[6] = (lrcb2 & 0x30)>>4 | 
  ( ( reportBuffer[6] & 0xc ) ) | 
  ( ( reportBuffer[6] & 0x3 ) << 6 ) |
  ( ( reportBuffer[6] & 0x30)  ) ;
}
}
else if ( gamePadType == 3 ) 
{	LATCH_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x80; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x40; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x20; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x01; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x02; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x04; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x08; }; CLOCK_HIGH();
CLOCK_LOW(); if (!GET_DATA() ) { tmp |= 0x10; }; CLOCK_HIGH();
reportBuffer[6] = tmp;
reportBuffer[0]=128; reportBuffer[1]=128;
CLOCK_LOW(); if (!GET_DATA() ) { reportBuffer[1]=0; };  CLOCK_HIGH();//Up
CLOCK_LOW(); if (!GET_DATA() ) { reportBuffer[0]=255; };CLOCK_HIGH();//Right
CLOCK_LOW(); if (!GET_DATA() ) { reportBuffer[1]=255; };CLOCK_HIGH();//Down
CLOCK_LOW(); if (!GET_DATA() ) { reportBuffer[0]=0; };  CLOCK_HIGH();//Left
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW();CLOCK_HIGH();
CLOCK_LOW(); if ( GET_DATA() ) { reportBuffer[6]  = 0x10; } CLOCK_HIGH();
LATCH_LOW();
}
}


static unsigned  char snesProbe(void)
{
unsigned char testData = 0;
CLOCK_BIT	=	(1<<0);
LATCH_BIT	=	(1<<1);
DATA_BIT	=	(1<<5);
DATA_PIN	=	PINB;
DATA_PORT	=	PORTB;
DATA_DDR	=	DDRB;
gamePadType = 1;
setUpPins();
_delay_ms(15);
for ( char i=0 ; i<6; i ++) 
{
delayClock (0, 2); delayClock (1, 2);if ( GET_DATA() ) testData++;																					
}

if (testData==1) { naki = 0; return 1;}
if (testData==4) { naki = 1; return 1;}
CLOCK_BIT	=	(1<<5);
LATCH_BIT	=	(1<<4);
DATA_BIT	=	(1<<3);
DATA_PIN	=	PINC;
DATA_PORT	=	PORTC;
DATA_DDR	=	DDRC;
gamePadType =	3;
setUpPins();
LATCH_HIGH();
testData=0;
for (char i=0; i<32; i++) 
{ 
CLOCK_LOW(); if ( !GET_DATA() && i > 16 ) { testData++; }	CLOCK_HIGH();
} 	
if ( testData==4 )  return 3;
LATCH_LOW();
testData=0;
LATCH_HIGH();_delay_us(12);LATCH_LOW();
for (char i=0; i<32; i++)  
{ 
CLOCK_LOW();_delay_us(6); if ( !GET_DATA() ) { testData++; }	CLOCK_HIGH();_delay_ms(6);
} 
if (testData==24 || testData==16) {  gamePadType = 2;  return 2; }
if (testData==18 )  { gamePadType = 4;  return 2; }
gamePadType = 0; return 0;
}
Gamepad SNES_Gamepad = {
.init					= snesInit,
.update					= snesUpdate,
.probe					= snesProbe
};
Gamepad *snesGetGamepad(void)
{
return &SNES_Gamepad;
}



