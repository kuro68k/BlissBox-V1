//copy of original code preamble 

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

 //this Code has been modifed see: Bliss-Box-Readme.txt

#include <util/delay.h>
#include "gamepad.h"
static inline unsigned char SAMPLE(void){
unsigned char c,b,res;c = PINC;b = PINB;
res =  (c & 0x20) >> 5;	res |= (c & 0x10) >> 3;	res |= (c & 0x08) >> 1;	res |= (c & 0x04) << 1;	res |= (c & 0x02) << 3;	res |= (b & 0x20); 
return res;}
#define SET_SELECT()	PORTC |= 1;
#define CLR_SELECT()	PORTC &= 0xfe;
static unsigned  char gamepad=0;
#define TG_SET_SELECT()	PORTC |= 0x02
#define TG_CLR_SELECT()	PORTC &= ~0x02
#define LOWWER_OE()		PORTC &= ~0x01
#define RISE_OE()		PORTC |= 0x01
static void db9Init(void);
static void db9Update(void);
static char db9Changed(void);
static unsigned  char db9Probe(void);
Gamepad *DB9GetGamepad(void);

#define CTL_ID_TG16 0x02 
#define CTL_ID_GENESIS3	0x03 
#define CTL_ID_GENESIS6 0x06
static unsigned  char cur_id = 3;

static char tg_readController()
{
char a,b;
LOWWER_OE();
_delay_ms(1);
TG_SET_SELECT();
_delay_ms(1);
a = (PINC & 0x3c) >> 2;
TG_CLR_SELECT();
_delay_ms(1);		
b = (PINC & 0x3c) >> 2;
RISE_OE();
return a | (b<<4);
}


static void readController( char bits[5])
{
char a,b,c,d,e;
SET_SELECT();
_delay_us(20.0);
a = SAMPLE();
CLR_SELECT();	_delay_us(20.0);		
b = SAMPLE();
SET_SELECT();	_delay_us(20.0);		
CLR_SELECT();	_delay_us(20.0);	
d = SAMPLE();
SET_SELECT();	_delay_us(20.0);		
CLR_SELECT();	_delay_us(20.0);
e = SAMPLE();
SET_SELECT();	_delay_us(20.0);			
c = SAMPLE();
CLR_SELECT();	_delay_us(20.0);			
SET_SELECT();
bits[0] = a;
bits[1] = b;
bits[2] = c;
bits[3] = d;
bits[4] = e;
}

static void db9Init(void)
{
if (cur_id == CTL_ID_TG16)
{
PORTC = 0x03; 
DDRC = 0x03;
return;
}
unsigned char bits[5];
setDB9Mode_sega();
DDRB &= ~0x20;
PORTB |= 0x20;
DDRC &= ~0x3E; 
PORTC |= 0x3F; 
if (cur_id == CTL_ID_GENESIS3) 
{
readController(bits);
if ((bits[0]&0xf) == 0xf) {
if ((bits[1]&0xf) == 0x3) 
{
if (	((bits[3] & 0xf) != 0x3)  ||
((bits[4] & 0xf) != 0x3) ) {
cur_id = CTL_ID_GENESIS6;
}
else {
cur_id = CTL_ID_GENESIS3;
}
}
}
}
}



static void db9Update(void)
{
char data[5], x=128,y=128;
if (cur_id == CTL_ID_TG16) 
{
data[1] = tg_readController();
data[1] ^= 0xff;
reportBuffer[6]=0;
if ((data[1] & 0xf) == 0xf)
{ 
gamepad =1;
if (data[1] & 0x80) reportBuffer[6] |= 0x10;
if (data[1] & 0x40) reportBuffer[6] |= 0x20;
if (data[1] & 0x20) reportBuffer[6] |= 0x40;
if (data[1] & 0x10) reportBuffer[6] |= 0x80;
data[1] = tg_readController();
data[1] ^= 0xff;
}

if (data[1] & 0x08) { y = 0; }
if (data[1] & 0x02) { y = 255; } 
if (data[1] & 0x01) { x = 0; }  
if (data[1] & 0x04) { x = 255; } 

reportBuffer[0]=x;
reportBuffer[1]=y;

if (data[1] & 0x80) reportBuffer[6] |= 0x01;
if (data[1] & 0x40) reportBuffer[6] |= 0x02;
if (data[1] & 0x20) reportBuffer[6] |= 0x04;
if (data[1] & 0x10) reportBuffer[6] |= 0x08;

}


readController(data);

if (cur_id == CTL_ID_GENESIS3) 
{
char tmp, tmp2, tmp3;
char data2[3];
char data3[3];
tmp = data[0] & (data[2] ^ 7) & 7;

if (tmp)
{

_delay_ms(1.5); 
readController(data2);

tmp2 = data2[0] & (data2[2] ^ 7) & tmp;
if (tmp2) 
{
_delay_ms(1.5);
readController(data3);
tmp3 = data3[0] & (data3[2] ^ 7) & tmp;

if (tmp3) cur_id = CTL_ID_GENESIS6;
}
}
}

data[0] = data[0] ^ 0xff;
data[1] = data[1] ^ 0xff;
data[2] = data[2] ^ 0xff;

if (data[0] & 1) { y = 0; } 
if (data[0] & 2) { y = 255; } 
if (data[0] & 4) { x = 0; }  
if (data[0] & 8) { x = 255; } 

reportBuffer[0]=x;
reportBuffer[1]=y;

reportBuffer[6]=0;

if (data[1]&0x10) { reportBuffer[6] |= 0x01; } 
if (data[0]&0x10) { reportBuffer[6] |= 0x02; } 
if (data[0]&0x20) { reportBuffer[6] |= 0x04; } 
if (data[1]&0x20) { reportBuffer[6] |= 0x08; } 
if (cur_id == CTL_ID_GENESIS6) 
{
if (data[2]&0x04) { reportBuffer[6] |= 0x10; } 
if (data[2]&0x02) { reportBuffer[6] |= 0x20; } 
if (data[2]&0x01) { reportBuffer[6] |= 0x40; } 
if (data[2]&0x08) { reportBuffer[6] |= 0x80; } 
}
}	

static unsigned  char db9Probe(void)
{
if (PINC == 3) { cur_id = CTL_ID_TG16; return 1;}
DDRC &= ~0x3E; 
PORTC |= 0x3F; 
char bits[3];
readController(bits);
if ((char)(bits[1] & 0xc))  cur_id = 0;
if (cur_id) {  cur_id = CTL_ID_GENESIS3; return 1; }	
SET_SELECT();
DDRD |= 0x08;
_delay_ms(1);
DDRC |= 0x02;
PORTC &= ~0x02;
PORTD &= ~0x08;
cur_id=0;
for (uint8_t i=0;i<10;i++) 
{		
PORTC |= 0x02;PORTC &= ~0x02; asm("nop\n ");
if (!(PINB & 0x20)) cur_id++;
}
return 0;		
}


Gamepad DB9Gamepad = {
.init					= db9Init,
.update					= db9Update,
.probe					= db9Probe,
};

Gamepad *db9GetGamepad(void)
{
return &DB9Gamepad;
}

