/* Name: main.c
 * Project: analog converter, will attempt to adapt the playstation from the example on v-usb's page
 * Author Sean Green
 * License: see v-usb examples lic.Original source code available here http://vusb.wikidot.com/project:ps2usb
 * Tabsize: 4
 * this Code has been modifed
 * I was able to adapt the ps2 code to do the following:
 * This code will support ps1, sp2, in digital and analog modes. There is also prevision for pressure buttons
 * here is how 
 * 
 * Only two buttons can be pressure active at once. the default is [] and X, if you want to change 
 * Press start hold it down ( this should pause the game ), then press select hold it down, and 
 * final Analog button Left ( button config 1) or analog button right ( button button config 1). So you will have one of the two.
 *
 * start+select+analog left
 * or
 * start+select+analog right
 *
 * Now while that combo is pressed ( almost impossible with one had when holding the controller , being the idea ) 
 * press the button you'd like. If you want L2 to have pressure for the extra HID analog 1 slot You would do:
 *
 * start+select+analog left+ L2 ( left go an any order )
 *
 * If you want Up to have pressure for the extra HID analog 2 slot You would do:
 *
 * start+select+analog right+ up( left go an any order )
 *///Guitar hero support is posible.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "gamepad.h"

/*********** prototypes *************/
static void psxInit(void);
static void psxUpdate(void);
static char psxProbe(void);
static void psxStartUp (void);
static void get_data(void) ;

Gamepad *psxGetGamepad(void);

#define ATT_ON() do {  PORTB &= ~0x02;} while(0)
#define ATT_OFF() do { PORTB |= 0x02; } while(0)

static char unsigned ps2buffer[21]={};
static unsigned char model=3; // 1 = ps1, 3 = ps2,  4 = unplugged

/* start up flag */
static unsigned char startUp =1;
static unsigned char autoPause=0;
static unsigned char pressureButton1=16,pressureButton2=15;//default pressure buttons
static char spi_mSend ( char podatek)	//straight from documentation
{
	// Gets information, sends it, waits untill it's sent, then reads the same register (used for both Input and Output) and returns it

	/* Start transmission */
	SPDR = podatek;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(100);
	return SPDR;
}

static void psxInit(void)
{
	DDRB = 0x2F;	// PORTB; everythin output, except MISO ( data )
	ATT_ON();	// ATT lines, uses CS and another pin
	/*
	-clk  pb5
	-cmd  pb4
	-data pb3
	-attn pb1 
	*/
}

static void psxStartUp (void)
{//this sets up the controller only done once, but too soon for init
 //this function will set up the controller to allow for pressure and sets the anolog LED
 //it is also ran if the user exits to digital and then returns.


	ATT_ON();
	spi_mSend(0x01);   // configure mode
	spi_mSend(0x43); 
	spi_mSend(0x00); 
	spi_mSend(0x01); // enter
	spi_mSend(0x00); 

	ATT_OFF();	
	_delay_us(255);


	ATT_ON();
	spi_mSend(0x01);  // set analog default mode ( does not work on all controllers )
	spi_mSend(0x44); 
	spi_mSend(0x00); 
	spi_mSend(0x01);  //analog.
	spi_mSend(0x00); 
	spi_mSend(0x00); 
	spi_mSend(0x00); 
	spi_mSend(0x00); 
	spi_mSend(0x00); 
	ATT_OFF();	

	_delay_us(255);
//Get more status info 

	ATT_ON();
	 spi_mSend(0x01);  
	 spi_mSend(0x45); 
	 spi_mSend(0x00);//ps1|pw2|GH TODO: Not sure how to tell a digital yet
   model=spi_mSend(0x5A);//1  | 3 | 1 TODO: how to tell from sp1 or gh? - answer check button 13 at start
	 spi_mSend(0x5A);//?  | 2 | 2
	 spi_mSend(0x5A);//0  | 0 | 0
	 spi_mSend(0x5A);//2  | 2 | 2
	 spi_mSend(0x5A);//1  | 1 | 1
	ps2buffer[2]=spi_mSend(0x5A);//?  | 0 | 0

	ATT_OFF();	
	if(model==0xff && autoPause < 20 )model=4;//no controller, autopause stops those flaky controlers that  report 0xff a lot

	_delay_us(255);

	ATT_ON();
	spi_mSend(0x01); // configure responces
	spi_mSend(0x4F); 
	spi_mSend(0x00); 
	spi_mSend(0xFF); 
	spi_mSend(0xFF); //turn everthing on. ( buttons, analogs and pressure )
	spi_mSend(0x03); 
	spi_mSend(0x00); 
	spi_mSend(0x00); 
	spi_mSend(0x00); 

	ATT_OFF();	

	_delay_us(255);


	ATT_ON();
	spi_mSend(0x01);  // configure mode
	spi_mSend(0x43);  
	spi_mSend(0x00);   
	spi_mSend(0x00);  //exit
	spi_mSend(0x5A);   
	spi_mSend(0x5A);   
	spi_mSend(0x5A);   
	spi_mSend(0x5A);   
	spi_mSend(0x5A);

	ATT_OFF();
	
}
static void get_data(void) 
{


	ATT_ON();
				 spi_mSend(0x01);  
	ps2buffer[1]=spi_mSend(0x42);  //get mode type
				 spi_mSend(0x00);  

	ps2buffer[3]=spi_mSend(0x00); //buttons
	ps2buffer[4]=spi_mSend(0x00); 

	if (ps2buffer[1]==0x41)
	{ // Digital mode
		model=1;
		ps2buffer[5]=ps2buffer[6]=1;//we are in digitial mode set a flag.	
	} 
	
	//in analog mode without pressure, since we want the pressure options all the time. lets set the startUp flag again.
	//should only get here if the user presses the analog button untill it goes red.
	//if this is a non prussure controller we need to allow this to check buttons



	else if ( ps2buffer[1] == 0x79 ||  ps2buffer[1] == 0x73 ) //some controller use other wierd numbers...but I dont want to fool with it
	{//in analog mode with pressure
		if(model==1) startUp=1;// analog button was off, need to run start up again ( for pressure)

		ps2buffer[5]=spi_mSend(0x00); //right_analog_x
		ps2buffer[6]=spi_mSend(0x00); //right_analog_y
		ps2buffer[7]=spi_mSend(0x00); //left_analog_x			
		ps2buffer[8]=spi_mSend(0x00); //left_analog_y

		//pressure
		ps2buffer[9] =spi_mSend(0x00); //R
		ps2buffer[10]=spi_mSend(0x00); //L
		ps2buffer[11]=spi_mSend(0x00); //U
		ps2buffer[12]=spi_mSend(0x00); //D
		ps2buffer[13]=spi_mSend(0x00); //Triangle
		ps2buffer[14]=spi_mSend(0x00); //O
		ps2buffer[15]=spi_mSend(0x00); //X
		ps2buffer[16]=spi_mSend(0x00); //Square
		ps2buffer[17]=spi_mSend(0x00); //L1
		ps2buffer[18]=spi_mSend(0x00); //R1
		ps2buffer[19]=spi_mSend(0x00); //L2
		ps2buffer[20]=spi_mSend(0x00); //R2
	}

	else if (ps2buffer[1] == 0xff && autoPause <20) //gives 20 tries before auto pause is disabled.


	{// posibly no controller , some controller report ff a lot so disable pause on them.
		model=4;
		autoPause ++;
	}


	ATT_OFF();	
	_delay_us(255);

	if ( (ps2buffer[1] != 0xff ) && startUp) 	
	{	
		startUp=0;// flag off
		psxStartUp();
	}

}
static  void psxUpdate(void)
{
	if (model==4 )
	{ 
		
		reportBuffer[6]=0x20;
		startUp=1;
		psxStartUp();
		return;
	}
	get_data(); 


	unsigned char temp1=0,temp2=0;
	reportBuffer[2]=ps2buffer[5];	// Right stick,  X axis
	reportBuffer[3]=ps2buffer[6];  // Right stick,  Y axis
	reportBuffer[0]=ps2buffer[7];	// Left stick, X axis
	reportBuffer[1]=ps2buffer[8];	// Left stick, Y axis
	//buttons
	if ( (255-ps2buffer[4]) & 0x80 ) temp1 |= 0x01;//X
	if ( (255-ps2buffer[4]) & 0x40 ) temp1 |= 0x02;//(square)
	if ( (255-ps2buffer[4]) & 0x10 ) temp1 |= 0x04;//(trangle)
	if ( (255-ps2buffer[4]) & 0x20 ) temp1 |= 0x08;//O
	if ( (255-ps2buffer[3]) & 0x1 )  temp1 |= 0x10;//select
	if ( (255-ps2buffer[3]) & 0x8 )  temp1 |= 0x20;//start
	if ( (255-ps2buffer[4]) & 0x8 )  temp1 |= 0x40;//R1
	if ( (255-ps2buffer[4]) & 0x2 )  temp1 |= 0x80;//R2

	if ( (255-ps2buffer[4]) & 0x4 ) temp2 |= 0x01;//L1
	if ( (255-ps2buffer[4]) & 0x1 ) temp2 |= 0x02;//L2
	if ( (255-ps2buffer[3]) & 0x10) temp2 |= 0x04;//up
	if ( (255-ps2buffer[3]) & 0x40) temp2 |= 0x08;//down
	if ( (255-ps2buffer[3]) & 0x80) temp2 |= 0x10;//left
	if ( (255-ps2buffer[3]) & 0x20) temp2 |= 0x20;//right
	if ( (255-ps2buffer[3]) & 0x2 ) temp2 |= 0x40;//analog stick button 1
	if ( (255-ps2buffer[3]) & 0x4 ) temp2 |= 0x80;//analog stick button 2
	reportBuffer[6]=temp1;
	reportBuffer[7]=temp2;
	
	//pressure define button1
	if ( ( (255-ps2buffer[3]) & 0xB ) == 0xB && !startUp)//avoid junk from starup messing with this.
	{
		if ( (255-ps2buffer[4]) & 0xFF ) //we dont want 0
		{
			pressureButton1= ( (255-ps2buffer[4]) & 0xFF );
			if (pressureButton1==1) pressureButton1=19;
			else if (pressureButton1==2) pressureButton1=20;
			else if (pressureButton1==4) pressureButton1=17;
			else if (pressureButton1==8) pressureButton1=18;
			else if (pressureButton1==16) pressureButton1=13;
			else if (pressureButton1==32) pressureButton1=14;
			else if (pressureButton1==64) pressureButton1=15;
			else if (pressureButton1==128) pressureButton1=16;
		}
		else if  ((255-ps2buffer[3]) & 0xF0 )//we dont want 0
		{
			pressureButton1= ( (255-ps2buffer[3]) & 0xF0 );
			if (pressureButton1==16) pressureButton1=11;
			else if (pressureButton1==32) pressureButton1=9;
			else if (pressureButton1==64) pressureButton1=12;
			else if (pressureButton1==128) pressureButton1=10;
		}
	}
	//pressure define button2
	if ( ( (255-ps2buffer[3]) & 0xD ) == 0xD && !startUp)//avoid junk from starup messing with this.
	{
		if ( (255-ps2buffer[4]) & 0xFF ) //we dnt want 0
		{
			pressureButton2= ( (255-ps2buffer[4]) & 0xFF );
			if (pressureButton2==1) pressureButton2=19;
			else if (pressureButton2==2) pressureButton2=20;
			else if (pressureButton2==4) pressureButton2=17;
			else if (pressureButton2==8) pressureButton2=18;
			else if (pressureButton2==16) pressureButton2=13;
			else if (pressureButton2==32) pressureButton2=14;
			else if (pressureButton2==64) pressureButton2=15;
			else if (pressureButton2==128) pressureButton2=16;
		}
		else if  ((255-ps2buffer[3]) & 0xF0 )//we dont want 0
		{
			pressureButton2= ( (255-ps2buffer[3]) & 0xF0 );
			if (pressureButton2==16) pressureButton2=11;
			else if (pressureButton2==32) pressureButton2=9;
			else if (pressureButton2==64) pressureButton2=12;
			else if (pressureButton2==128) pressureButton2=10;
		}
	}

	//pressure
	reportBuffer[5]=ps2buffer[pressureButton1];
	reportBuffer[4]=ps2buffer[pressureButton2];

	if (ps2buffer[5]+ps2buffer[6] == 2)// Digital pad
	{
		// clear all
		reportBuffer[0]=128;
		reportBuffer[1]=128;
		reportBuffer[6]=0;
		reportBuffer[7]=0;
		reportBuffer[4]=0;
		reportBuffer[5]=0;
		temp1=0,temp2=0;

	//buttons
		if ( (255-ps2buffer[4]) & 0x80 ) temp1 |= 0x01;//X
		if ( (255-ps2buffer[4]) & 0x40 ) temp1 |= 0x02;//(square)
		if ( (255-ps2buffer[4]) & 0x10 ) temp1 |= 0x04;//(trangle)
		if ( (255-ps2buffer[4]) & 0x20 ) temp1 |= 0x08;//O
		if ( (255-ps2buffer[3]) & 0x1 )  temp1 |= 0x10;//select
		if ( (255-ps2buffer[3]) & 0x8 )  temp1 |= 0x20;//start
		if ( (255-ps2buffer[4]) & 0x8 )  temp1 |= 0x40;//R1
		if ( (255-ps2buffer[4]) & 0x2 )  temp1 |= 0x80;//R2

		if ( (255-ps2buffer[4]) & 0x4 ) temp2 |= 0x01;//L1
		if ( (255-ps2buffer[4]) & 0x1 ) temp2 |= 0x02;//L2
		if ( (255-ps2buffer[3]) & 0x10) reportBuffer[1]= 0x00;//up
		if ( (255-ps2buffer[3]) & 0x40) reportBuffer[1]= 0xFF;//down
		if ( (255-ps2buffer[3]) & 0x80) reportBuffer[0]= 0x00;//left
		if ( (255-ps2buffer[3]) & 0x20) reportBuffer[0]= 0xFF;//right
		reportBuffer[6]=temp1;
		reportBuffer[7]=temp2;	
	}

}

static char psxProbe(void)
{
//seems psx does not always kick in right a way, and no delay is helping..So try a few

	for (char i=0;i<10;i++)// Give it 10 tries. ( could be less but 2 is not allways enough )
	{
		ATT_ON();
		spi_mSend(0x01); 
		 char read = spi_mSend(0x42);
			spi_mSend(0x00); 
			spi_mSend(0x00); 
			spi_mSend(0x00);

			ATT_OFF();


		if ( read > 1 && read < 255 )// not 0 and not ff...was: ( spi_mSend(0x42) == 0x41 || spi_mSend(0x42) == 0x73 ) 
		return 1;
		_delay_ms(1);//compatibility ( needed)
	}
	return 0;
}



Gamepad PSX_Gamepad = {
	.init					= psxInit,
	.update					= psxUpdate,
	.probe					= psxProbe,
};

Gamepad *psxGetGamepad(void)
{
	return &PSX_Gamepad;
}


