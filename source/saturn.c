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
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "gamepad.h"
/*********** prototypes *************/
static void saturnInit(void);
static void saturnUpdate(void);
static  char pinStatus ( unsigned  char pin) __attribute__((always_inline));
Gamepad *saturnGetGamepad(void);
static unsigned char saturnPad = 1;
static inline char pinStatus( unsigned  char pin ){ return ~PINC & pin; }
static void saturnInit(void)
{
DDRC = 0x3;  
PORTC |= 0x3f;
_delay_ms(100);
}
static void saturnUpdate(void)
{
unsigned char  data= 0,data2= 0; 
unsigned char  x=128,y=128,l=128,r=128,dir_x=128,dir_y=128,tempBits=0, tempBits2=0, digital=0;		
if (saturnPad==2)
{
_delay_ms(20);
PORTC  |= 0x3;	_delay_us(7);
PORTC &= 0x3D;	_delay_us(7);
if ( (PINC>>2) != 0x8 )
{
reportBuffer[6]=0x08;
reportBuffer[7]=data2;
return;
}
for ( char  i = 0; i< 14;i ++)
{
_delay_us(7);
if ( PORTC & (1<<0) ) PORTC &= 0x3C;
else 			  PORTC  |= 0x1;
_delay_us(7);
if (i==2)
{ 
if ( pinStatus(1<<5) ) { data2 |= 0x02; dir_y=0;  }
if ( pinStatus(1<<4) ) { data2 |= 0x04; dir_y=255;}
if ( pinStatus(1<<3) ) { data2 |= 0x08; dir_x=0;  }
if ( pinStatus(1<<2) ) { data2 |= 0x10; dir_x=255;}
}
else  if (i==3)
{ 
if ( pinStatus(1<<5) ) data |= 0x02; 
if ( pinStatus(1<<4) ) data |= 0x04; 
if ( pinStatus(1<<3) ) data |= 0x01; 
if ( pinStatus(1<<2) ) data |= 0x08;
}
else if (i==4)
{ 
if ( pinStatus(1<<5) ) data  |= 0x40; 
if ( pinStatus(1<<4) ) data  |= 0x20; 
if ( pinStatus(1<<3) ) data  |= 0x10; 
if ( pinStatus(1<<2) ) data2 |= 0x01; 
}
else if (i==5)
{ 
if ( pinStatus(1<<2) ) data |= 0x80; 
if ( pinStatus(1<<3) ) asm("nop\n "); 
if ( pinStatus(1<<4) ) asm("nop\n "); 
if ( pinStatus(1<<5) ) asm("nop\n ");
}
else if (i==6) { tempBits = PINC>>2;  }
else if (i==7)
{
digital += PINC>>2;
tempBits2 = PINC>>2;
x = tempBits | tempBits2<<4;
x = ((x * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}
else if (i==8) { tempBits = PINC>>2; }
else if (i==9)
{
digital += PINC>>2;
tempBits2 = PINC>>2;
y = tempBits | tempBits2<<4;
y = ((y * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}
else if (i==10) { tempBits = PINC>>2;  }
else if (i==11)
{
digital += PINC>>2;
tempBits2 = PINC>>2;
l = tempBits | tempBits2<<4;
l = ((l * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}
else if (i==12) {tempBits = PINC>>2;  }
else if (i==13)
{
digital += PINC>>2;
tempBits2 = PINC>>2;
r = tempBits | tempBits2<<4;
r = ((r * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}
}	
if( digital ==32 ) 
{ 
x=dir_x; 
y=dir_y; 
data2 &= 0x1;
l=r=128;
}
PORTC  |= 0x3;	_delay_us(7); 
} 
else if (saturnPad==1)
{
if ( pinStatus(1<<2) ) data |= 0x80; 
PORTC &= 0x3E;_delay_us(7); 
if ( pinStatus(1<<5) ) data |= 0x2; 
if ( pinStatus(1<<4) ) data |= 0x4;
if ( pinStatus(1<<3) ) data |= 0x1; 
if ( pinStatus(1<<2) ) data |= 0x8; 
PORTC |= 0x3;
PORTC &= 0x3D;_delay_us(7);
if ( pinStatus(1<<5) ) y = 0;   
if ( pinStatus(1<<4) ) y = 255; 
if ( pinStatus(1<<3) ) x = 0;   
if ( pinStatus(1<<2) ) x = 255; 
PORTC &= 0x3C;_delay_us(7);
if ( pinStatus(1<<5) ) data |= 0x10; 
if ( pinStatus(1<<4) ) data |= 0x20; 
if ( pinStatus(1<<3) ) data |= 0x40; 
if ( pinStatus(1<<2) ) data2 |=0x01; 
PORTC  |= 0x3;_delay_us(7);
}
reportBuffer[0]=x;
reportBuffer[1]=y;
reportBuffer[4]=l;
reportBuffer[5]=r;
reportBuffer[6]=data;
reportBuffer[7]=data2;
}
static char saturnProbe(void)
{
PORTC  |= 0x3;	_delay_us(7); 
if( (PINC>>2) == 0x3 || (PINC>>2) == 0x2) { saturnPad=1; return 1;}; 
}
Gamepad SATURN_Gamepad = {
.init					= saturnInit,
.update					= saturnUpdate,
.probe					= saturnProbe,
};
Gamepad *saturnGetGamepad(void)
{
return &SATURN_Gamepad;
}
