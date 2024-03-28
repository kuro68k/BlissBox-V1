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
#define __SREG__ _SFR_IO_ADDR(SREG)
#define PINx	_SFR_IO_ADDR(PINC)
#define DEBUG	_SFR_IO_ADDR(PORTB)
#define SCKA	5
#define SCKB	4
.text
.extern sega_buffer
.extern rejected
.global	PCINT1_vect
PCINT1_vect:
sbic 	PINx,SCKA
reti
ldi	r25,0
sts PCICR,r25;
sts PCMSK1,r25;
sbis 	PINx,SCKA
rjmp PCINT1_vect
push r0
in r0,__SREG__
push r0				
push r18			
push r19			
push r26			
push r27
ldi		r26,lo8(sega_buffer)
ldi		r27,hi8(sega_buffer)
sub		r18,r18	
ldi		r19,4
ldi r25,179
loop:	 
dec r25
brne loop
nop
edge:
sbis 	PINx,SCKB
rjmp edge
nextloop:
ldi r25,2
loop2:	 
dec r25
brne loop2
nop
nextBit:
nop
nop
nop
sbic	PINx,SCKB
sec
st		X+,r18
clr		r18
rol		r18
nop 
nop
nop
sbic	PINx,SCKA
sec
rol		r18
ldi r25,2
skip5:
dec r25
brne skip5
sbic	PINx,SCKB
sec
rol		r18
ldi r25,2
skip4:
dec r25
brne skip4
sbic	PINx,SCKA		
sec
rol		r18
ldi r25,2
skip3:
dec r25
brne skip3
sbic	PINx,SCKB	
sec
rol		r18
ldi r25,2
skip2:
dec r25
brne skip2
sbic	PINx,SCKA		
sec
rol		r18	
ldi r25,2
skip1:
dec r25
brne skip1
sbic	PINx,SCKB		
sec
rol		r18

ldi r25,2
skip0:
dec r25
brne skip0
sbic	PINx,SCKA	
sec
rol		r18
dec		r19
brne nextBit 
ldi		r19,5
analog_nextBit:
sbic	PINx,SCKB
sec
st		X+,r18
clr		r18
rol		r18
nop
nop
nop
sbic	PINx,SCKA
sec
rol		r18
ldi r25,2;
analog_skip5:
dec r25
brne analog_skip5
sbic	PINx,SCKB	
sec
rol		r18
ldi r25,2;
analog_skip4:
dec r25
brne analog_skip4
sbic	PINx,SCKA		
sec
rol		r18
ldi r25,2
analog_skip3:
dec r25
brne analog_skip3
sbic	PINx,SCKB
sec
rol		r18
ldi r25,2
analog_skip2:
dec r25
brne analog_skip2

sbic	PINx,SCKA
sec
rol		r18	
ldi r25,2;
analog_skip1:
dec r25
brne analog_skip1
sbic	PINx,SCKB
sec
rol		r18
ldi r25,2
analog_skip0:
dec r25
brne analog_skip0
sbic	PINx,SCKA
sec
rol		r18
dec		r19
breq EXIT
sbic	PINx,SCKA
jmp analog_nextBit
jmp analog_nextBit
EXIT:
pop r27
pop r26
pop r19				
pop r18
pop r0
out __SREG__,r0
pop r0
reti
EXIT2:
ldi r25,200
timer:
dec r25
brne timer
ldi r25,200; 
timer2:
dec r25
brne timer2
ldi r25,200; 
timer3:
dec r25
brne timer3
reti



