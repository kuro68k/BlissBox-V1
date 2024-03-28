//seag green : Aug 21st 2008 -  merged reportdesc.h in to this header
#define DEBUG_LEVEL 0
#ifndef _gamepad_h__
#define _gamepad_h__

#include <avr/pgmspace.h>

extern const char usbDescrDevice[] PROGMEM;
int getUsbDescrDevice_size(void);

#define ANALOG_REPORT_SIZE		8

extern const char analog_usbHidReportDescriptor[] PROGMEM ;

volatile char _vectrex ;

volatile char sega_buffer[9];// for the dreacast

volatile char reportBuffer[8];    /* buffer for HID reports */
//#define AnalogUsbHidReportDescriptor_size sizeof(analog_usbHidReportDescriptor)

int getAnalogUsbHidReportDescriptor_size(void);

#define setDB9Mode_sega() 			{ DDRD |= 0x18; PORTD &= ~0x10; PORTD |=  0x08; } 
#define setDB9Mode_atari_normal()	{ DDRD |= 0x10; PORTD &= ~0x10; PORTC |=  0x01; } 
#define setDB9Mode_atari_reversed()	{ DDRD |= 0x10; PORTD |=  0x10; PORTC &= ~0x01; } 

#define LED_OFF() do { PORTD |= 0x20; } while(0)
#define LED_ON() do { PORTD &= ~0x20; } while(0)
static void blinkLed(int n_times)
{
if (n_times == 0 )  return;
char i;
DDRD |= 0x20;
	LED_OFF();
	while (n_times--)
	{
		LED_ON();
		for (i=0; i<20; i++) _delay_ms(10);
		LED_OFF();
		for (i=0; i<50; i++) _delay_ms(10);
	}	

}

typedef struct {
	// size of reports built by buildReport
	void (*init)(void);
	void (*update)(void);
	char (*probe)(void); 
} Gamepad;

#endif // _gamepad_h__
 

