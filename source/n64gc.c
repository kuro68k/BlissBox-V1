//copy of original code preamble 

/* Name: main.c
 * Project: Gamecube/n64 to USB converter
 * Author: Raphael Assenat <raph@raphnet.net
 * Copyright: (C) 2007 Raphael Assenat <raph@raphnet.net>
 * License: Proprietary, free under certain conditions. See Documentation.
 * Tabsize: 4
 */

  //this Code has been modifed see: Bliss-Box-Readme.txt


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "gamepad.h"


/******** IO port definitions **************/
#define DATA_PORT	PORTC
#define DATA_DDR	DDRC
#define DATA_PIN	PINC
#define DATA_BIT	(1<<5)

/*********** prototypes *************/
static void Init(void);
static void Update(void);
static unsigned char Probe(void);


Gamepad *nGetGamepad(void);

static unsigned char GC = 0;

/* For probe */
static unsigned  char last_failed;

static void Init(void)
{
	// data as input
	DATA_DDR &= ~(DATA_BIT);

	// keep data low. By toggling the direction, we make the
	// pin act as an open-drain output.
	DATA_PORT &= ~DATA_BIT;

}

static void gamecubeUpdate(void)
{
	unsigned char tmp=0;
	unsigned char tmpdata[8];	
	unsigned char volatile results[65];
	unsigned char count;
	
	tmp = 0x00; // get status
	/*
	 * z: Points to current source byte
	 * r16: Holds the bit to be AND'ed with current byte
	 * r17: Holds the current byte
	 * r18: Holds the number of bits left to send.
	 * r19: Loop counter for delays
	 *
	 * Edit sbi/cbi/andi instructions to use the right bit!
	 * 
	 * 3 us == 36 cycles
	 * 1 us == 12 cycles
	 *
	 * Note: I know the code below is not very clean... but it works.
	 */

	asm volatile(
"			push r30				\n"
"			push r31				\n"

"			ldi r16, 0x80			\n" // 1
"nextBit:							\n" 
"			ldi r17, 0x40				\n" // 2		// BITS 0-7 (MSB sent first)
"			and r17, r16			\n" // 1
"			breq send0				\n" // 2
"			nop						\n"

"send1:								\n"
"			sbi %1, 5				\n" // 2
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\n			\n" // 3
"			cbi %1, 5				\n" // 2
"				ldi r19, 8			\n"	// 1 
"lp1:			dec r19				\n"	// 1 
"				brne lp1			\n"	// 2
"				nop\nnop			\n" // 2
"			lsr r16					\n" // 1
"			breq done				\n" // 1
"			rjmp nextBit			\n" // 2
		
"send0:		sbi %1, 5				\n"	// 2
"				ldi r19, 11			\n"	// 1
"lp0:			dec r19				\n"	// 1
"				brne lp0			\n"	// 2
"				nop					\n" // 1

"          	cbi %1,5				\n" // 2
"				nop\nnop\n				\n" // 2

"			lsr r16					\n" // 1
"			breq done				\n" // 1
"			rjmp nextBit			\n" // 2

"done:								\n"
"			cbi %1, 5				\n"


///////////////////////////////////////////// BITS 8-15

"			ldi r16, 0x80			\n" // 1
"nextBitA:							\n" 
"			ldi r17, 0x03			\n" // 2			// BITS 8-15
"			and r17, r16			\n" // 1
"			breq send0A				\n" // 2
"			nop						\n"

//"send1:								\n"
"			sbi %1, 5				\n" // 2
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\n			\n" // 3
"			cbi %1, 5				\n" // 2
"				ldi r19, 8			\n"	// 1 
"lp1A:			dec r19				\n"	// 1 
"				brne lp1A			\n"	// 2
"				nop\nnop			\n" // 2
"			lsr r16					\n" // 1
"			breq doneA				\n" // 1
"			rjmp nextBitA			\n" // 2
		
"send0A:	sbi %1, 5				\n"	// 2
"				ldi r19, 11			\n"	// 1
"lp0A:			dec r19				\n"	// 1
"				brne lp0A			\n"	// 2
"				nop					\n" // 1

"          	cbi %1,5				\n" // 2
"				nop\nnop\n				\n" // 2

"			lsr r16					\n" // 1
"			breq doneA				\n" // 1
"			rjmp nextBitA			\n" // 2

"doneA:								\n"
"			cbi %1, 5				\n"

////////////////////////// BITS 16-23
"			ldi r16, 0x80			\n" // 1
"nextBitB:							\n" 
"			mov r17, %2				\n" // 2 		// BITS 16-23
"			and r17, r16			\n" // 1
"			breq send0B				\n" // 2
"			nop						\n"

//"send1B:								\n"
"			sbi %1, 5				\n" // 2
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\n			\n" // 3
"			cbi %1, 5				\n" // 2
"				ldi r19, 8			\n"	// 1 
"lp1B:			dec r19				\n"	// 1 
"				brne lp1B			\n"	// 2
"				nop\nnop			\n" // 2
"			lsr r16					\n" // 1
"			breq doneB				\n" // 1
"			rjmp nextBitB			\n" // 2
		
"send0B:	sbi %1, 5				\n"	// 2
"				ldi r19, 11			\n"	// 1
"lp0B:			dec r19				\n"	// 1
"				brne lp0B			\n"	// 2
"				nop					\n" // 1

"          	cbi %1,5				\n" // 2
"				nop\nnop\n				\n" // 2

"			lsr r16					\n" // 1
"			breq doneB				\n" // 1
"			rjmp nextBitB			\n" // 2

"doneB:								\n"
"			cbi %1, 5				\n"
"			nop\nnop\nnop\nnop		\n"	// 4


// Stop bit (1us low, 3us high)
"          	sbi %1,5				\n" // 2
"			nop\nnop\nnop\nnop		\n" // 4
"			cbi %1,5				\n" 

"anti_slow_rise:\n"
"			in r17, %4\n"
"           andi r17, 0x20\n"
"           breq anti_slow_rise\n"

			// Response reading part //
			// r16: loop Counter
			// r17: Reference state
			// r18: Current state
			// r19: bit counter
		
"			ldi r19, 64\n			" // 1  We will receive 32 bits

"st:\n								"
"			ldi r16, 0x3f			\n" // setup a timeout
"waitFall:							\n" 
"			dec r16					\n" // 1
"			breq timeout			\n" // 1 
"			in r17, %4				\n" // 1  read the input port
"			andi r17, 0x20			\n" // 1  isolate the input bit
"			brne waitFall			\n" // 2  if still high, loop

// OK, so there is now a 0 on the wire.
// Worst case, we are at the 9th cycle.
// Best case, we are at the 4th cycle.
// 	Middle: cycle 6
// 
// cycle: 1-12 12-24 24-36 36-48
//  high:  0     1     1     1
//	 low:  0     0     0     1
//
// I check the pin on the 24th cycle which is the safest place.

"			cbi %5, 1\n"				// DEBUG
"			nop\nnop\n	" // 2
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\n"
			
			// We are now more or less aligned on the 24th cycle.			
"			in r18, %4\n			" // 1  Read from the port
"			sbi %5, 1\n"				// DEBUG
"			andi r18, 0x20\n		" // 1  Isolate our bit
"			st z+, r18\n			" // 2  store the value

"			dec r19\n				" // 1 decrement the bit counter
"			breq ok\n				" // 1

"			ldi r16, 0x3f\n			" // setup a timeout
"waitHigh:\n						"
"			dec r16\n				" // decrement timeout
"			breq timeout\n			" // handle timeout condition
"			in r18, %4\n			" // Read the port
"			andi r18, 0x20\n		" // Isolate our bit
"			brne st\n				" // Continue if set
"			rjmp waitHigh\n			" // loop otherwise..

"ok:\n"
"			clr %0\n				"
"			rjmp end\n				"

"error:\n"
"			sbi %5, 1\n"
"			mov %0, r19				\n" // report how many bits were read
"			rjmp end				\n"

"timeout:							\n"
"			clr %0					\n"
"			com %0					\n" // 255 is timeout

"end:\n"
"			pop r31\n"
"			pop r30\n"
"			cbi %5, 1\n"
			: "=r" (count)
			: "I" (_SFR_IO_ADDR(DATA_DDR)), "r"(tmp), 
				"z"(results), "I" (_SFR_IO_ADDR(DATA_PIN)),
				"I" (_SFR_IO_ADDR(PORTB))
			: "r16","r17","r18","r19"
			);


	//should never really happpen twice in a row...
	if ( last_failed && count == 255 ) //pause check.
	{
		reportBuffer[6] = 0x10;
		return;
	}

	if (count == 255) {
		last_failed = 1;
		return; // failure
	}
	else last_failed = 0;//'reset
	
	
/*
	(Source: Nintendo Gamecube Controller Protocol
		updated 8th March 2004, by James.)

	Bit		Function
	0-2		Always 0 
	3		Start
	4		Y
	5		X
	6		B
	7		A
	8		Always 1
	9		L
	10		R
	11		Z
	12-15	Up,Down,Right,Left
	16-23	Joy X
	24-31	Joy Y
	32-39	C Joystick X
	40-47	C Joystick Y
	48-55	Left Btn Val
	56-63	Right Btn Val
 */
	
	/* Convert the one-byte-per-bit data generated
	 * by the assembler mess above to nicely packed
	 * binary data. */	
	memset(tmpdata, 0, sizeof(tmpdata));

	for (uint8_t i=0; i<8; i++) 
	{
		tmpdata[0] |= results[i+16] ? (0x80>>i) : 0;// X axis
		tmpdata[1] |= results[i+24] ? (0x80>>i) : 0;// Y axis
		tmpdata[2] |= results[i+32] ? (0x80>>i) : 0;// C X axis
		tmpdata[3] |= results[i+40] ? 0 : (0x80>>i);// C Y axis	
		tmpdata[5] |= results[i+48] ? 0 : (0x80>>i);// Left btn value
		tmpdata[4] |= results[i+56] ? 0 : (0x80>>i);// Right btn value	
	}
	tmpdata[4] ^= 0xff;
	tmpdata[1] ^= 0xff;
	tmpdata[5] ^= 0xff;


	for (uint8_t i=6; i>0; i--)	tmpdata[6] |= results[i+2] ? (0x20>>i) : 0; //A B Y X ST
	for (uint8_t i=0; i<3; i++) tmpdata[6] |= results[i+9] ? (0x20<<i) : 0;// L R Z
	
	for (uint8_t i=0; i<4; i++)	tmpdata[7] |= results[i+12] ? (0x01<<i) : 0; // Up,Down,Right,Left

	// analog joysticks
	for (uint8_t i=0; i<6; i++) reportBuffer[i] = tmpdata[i];
		
	// buttons
	reportBuffer[6] = tmpdata[6];
	reportBuffer[7] = tmpdata[7];

}

static void n64Update( unsigned  char tmp)
{
	if (GC) { gamecubeUpdate(); return; }

	unsigned char tmpdata[4];	
	unsigned char volatile results[65];
	unsigned char count;
	

	/*
	 * z: Points to current source byte
	 * r16: Holds the bit to be AND'ed with current byte
	 * r17: Holds the current byte
	 * r18: Holds the number of bits left to send.
	 * r19: Loop counter for delays
	 *
	 * Edit sbi/cbi/andi instructions to use the right bit!
	 * 
	 * 3 us == 36 cycles
	 * 1 us == 12 cycles
	 */
	asm volatile(
"			push r30				\n"
"			push r31				\n"
			
"			ldi r16, 0x80			\n" // 1
"nextBitGB:							\n" 
"			mov r17, %2				\n" // 2
//"			ldi r17, 0x0f			\n" // 2
"			and r17, r16			\n" // 1
"			breq send0GB				\n" // 2
"			nop						\n"

"send1GB:								\n"
"			sbi %1, 5				\n" // 2
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\nnop\nnop	\n" // 4
"				nop\nnop\n			\n" // 3
"			cbi %1, 5				\n" // 2
"				ldi r19, 8			\n"	// 1 
"lp1GB:			dec r19				\n"	// 1 
"				brne lp1GB			\n"	// 2
"				nop\nnop			\n" // 2
"			lsr r16					\n" // 1
"			breq doneGB				\n" // 1
"			rjmp nextBitGB			\n" // 2
		
/* Send a 0: 3us Low, 1us High */
"send0GB:		sbi %1, 5				\n"	// 2
"				ldi r19, 11			\n"	// 1
"lp0GB:			dec r19				\n"	// 1
"				brne lp0GB			\n"	// 2
"				nop					\n" // 1

"          	cbi %1,5				\n" // 2
"				nop\nnop\n				\n" // 2

"			lsr r16					\n" // 1
"			breq doneGB				\n" // 1
"			rjmp nextBitGB			\n" // 2

"doneGB:								\n"
"			cbi %1, 5				\n"
"			nop\nnop\nnop\nnop		\n"	// 4
"			nop\nnop\nnop\nnop		\n"	// 4
"			nop						\n"	// 1


// Stop bit (1us low, 3us high)
"          	sbi %1,5				\n" // 2
"			nop\nnop\nnop\nnop		\n" // 4
"			cbi %1,5				\n" 
			

			// Response reading part //
			// r16: loop Counter
			// r17: Reference state
			// r18: Current state
			// r19: bit counter
		
"			ldi r19, 33\n			" // 1  We will receive 32 bits

"stGB:\n								"
"			ldi r16, 0xff			\n" // setup a timeout
"waitFallGB:							\n" 
"			dec r16					\n" // 1
"			breq timeoutGB			\n" // 1 
"			in r17, %4				\n" // 1  read the input port
"			andi r17, 0x20			\n" // 1  isolate the input bit
"			brne waitFallGB			\n" // 2  if still high, loop

// OK, so there is now a 0 on the wire.
// Worst case, we are at the 9th cycle.
// Best case, we are at the 4th cycle.
// 	Middle: cycle 6
// 
// cycle: 1-12 12-24 24-36 36-48
//  high:  0     1     1     1
//	 low:  0     0     0     1
//
// I check the pin on the 24th cycle which is the safest place.

"			cbi %5, 1\n"				// DEBUG
"			nop\nnop\n	" // 2
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\nnop\nnop\nnop\n	" // 4
"			nop\n					"
			
			// We are now more or less aligned on the 24th cycle.			
"			in r18, %4\n			" // 1  Read from the port
"			sbi %5, 1\n"				// DEBUG
"			andi r18, 0x20\n		" // 1  Isolate our bit
"			st z+, r18\n			" // 2  store the value

"			dec r19\n				" // 1 decrement the bit counter
"			breq okGB\n				" // 1

"			ldi r16, 0xff\n			" // setup a timeout
"waitHighGB:\n						"
"			dec r16\n				" // decrement timeout
"			breq timeoutGB\n			" // handle timeout condition
"			in r18, %4\n			" // Read the port
"			andi r18, 0x20\n		" // Isolate our bit
"			brne stGB\n				" // Continue if set
"			rjmp waitHighGB\n			" // loop otherwise..

"okGB:\n"
"			clr %0\n				"
"			rjmp endGB\n				"

"errorGB:\n"
"			sbi %5, 1\n"
"			mov %0, r19				\n" // report how many bits were read
"			rjmp endGB				\n"

"timeoutGB:							\n"
"			clr %0					\n"
"			com %0					\n" // 255 is timeout

"endGB:\n"
"			pop r31\n"
"			pop r30\n"
"			cbi %5, 1\n"
			: "=r" (count)
			: "I" (_SFR_IO_ADDR(DATA_DDR)), "r"(tmp), 
				"z"(results), "I" (_SFR_IO_ADDR(DATA_PIN)),
				"I" (_SFR_IO_ADDR(PORTB))
			: "r16","r17","r18","r19"
			);

	//should never really happpen twice in a row...
	if ( last_failed && count == 255 ) //pause check.
	{
		reportBuffer[6] = 0x08;
		return;
	}


	if (count == 255) {
		last_failed =1;
		return; // failure
	}
	else last_failed=0;//reset
	
	
/*
	Bit	Function
	0	A
	1	B
	2	Z
	3	Start
	4	Directional Up
	5	Directional Down
	6	Directional Left
	7	Directional Right
	8	unknown (always 0)
	9	unknown (always 0)
	10	L
	11	R
	12	C Up
	13	C Down
	14	C Left
	15	C Right
	16-23: analog X axis
	24-31: analog Y axis
 */

	tmpdata[0]=0;
	tmpdata[1]=0;
	tmpdata[2]=0;
	tmpdata[3]=0;


	for (uint8_t i=0; i<4; i++) 
	{
		tmpdata[2] |= results[i] ? (0x01<<i) : 0;// A B Z START
		tmpdata[2] |= results[i+12] ? (0x10<<i) : 0; // C-UP C-DOWN C-LEFT C-RIGHT
	}

	for (uint8_t i=0; i<2; i++) // L R
		tmpdata[3] |= results[i+10] ? (0x01<<i) : 0;

	for (uint8_t i=0; i<8; i++) 
	{
		tmpdata[0] |= results[i+16] ? (0x80>>i) : 0;// X axis
		tmpdata[1] |= results[i+24] ? (0x80>>i) : 0;// Y axis
	}



	// analog joystick
	reportBuffer[0] = ((int)((signed char)tmpdata[0]))+127;
	reportBuffer[1] = ((int)( -((signed char)tmpdata[1])) )+127;

	// buttons
	reportBuffer[6] = tmpdata[2];
	reportBuffer[7] = tmpdata[3];

	// dpad as buttons
	if (results[4]) 
		reportBuffer[7] |= 0x04;
	if (results[5])
		reportBuffer[7] |= 0x08;
	if (results[6])
		reportBuffer[7] |= 0x10;
	if (results[7])
		reportBuffer[7] |= 0x20;

}

static void Update(void)
{
	n64Update(0x01); // get status
}
static unsigned  char Probe(void)
{


	for (uint8_t i=0; i<15; i++)
	{
		_delay_ms(14);
		
		last_failed = 0;

		n64Update(0x01);
		if (!last_failed) { GC = 0; return 1; }

		gamecubeUpdate();
		if (!last_failed) { GC = 1; return 1; }
	}


	return 0;
}

Gamepad nGamepad = {
	.init					= Init,
	.update					= Update,
	.probe					= Probe,
};

Gamepad *nGetGamepad(void)
{
	return &nGamepad;
}

