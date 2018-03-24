//common.c
/*****************************************************************************************
 * Project:         Continuity Tester
 * Version:         V1.0
 * Date Created:    03/20/2018
 * Date Modified:   03/22/2018
 * Author:          Siddharth Singh
 * Project URL:     https://hackaday.io/project/96603-continuity-diode-and-led-tester
 
 * Micro:           PIC 10LF322
 * Speed:           16MHz
 * Clock Source:    Internal
 * IDE:				MPLAB v8.92.00.00
 * Compiler:		Hi-Tech v9.81
 *****************************************************************************************/

#include <htc.h>
#include "common.h"



__CONFIG(0x3C80); // configuration bits

//=============================================================================
//								Global Variables
//=============================================================================
bit sleepFlag=0;
unsigned char sec=0,mode=1,clk; 
bit keyFlag=0,longKeyFlag=0,keyRead=0;

//=============================================================================
//					Hardware and Interrupt Initialization
//=============================================================================

void initHardware()
{
	// Hardware Initialization
		OSCCON=0b01110000; 		//Oscillator clock config (pg-26 datasheet) CLK 16MHz
		OPTION_REG = 0b00000101;	// pg-95 datasheet. prescaler 1:64

	//	nWPUEN=0;	//weak pullup resistor enable bit, 0=enable, 1=disable
		WPUA=0b00001000;	// port RA3 (pushSwitch) weak pullup enabled

	//INTCON Configuration
		//	IOCIE=1;			//Interrupt on change enable
		//	IOCAP=0b00000000;	// INTERRUPT-ON-CHANGE PORTA POSITIVE EDGE REGISTER (pg 75 datasheet)
		//	IOCAN=0b00001000;	// INTERRUPT-ON-CHANGE PORTA NEGATIVE EDGE REGISTER (pg 75 datasheet) pushSwitch RA3 enabled falling edge
		//	IOCAF=0b00000000; 	//INTERRUPT-ON-CHANGE PORTA FLAG REGISTER (pg 76 datasheet)
			TMR0IE=1;			//Timer0 Overflow Interrupt Enable bit
			TMR0IF=0;			//Timer0 Overflow Interrupt Flag bit
	
	//PWM and Timer2 Configuration
	
		PWM1CON=0;
		PR2=98;		//PWM freq 10KHz
		PWM1PW=0;
		PWM1PWL=0;
		T2CON=0b00000001; 	//Timer2  prescaler 1:4
		PEIE=1;				//Peripheral interrupt enable bit
		TMR2IE=1;			//Timer2 interrupt enable bit
	
	
	GIE=1;	//Global interrupt enable


} //end initHardware()

//=============================================================================
//	                 		ADC Read Routine, returns 8 bit ADC value
//=============================================================================

unsigned char readADC()
{
	__delay_us(50);		//sampling interval
	GO_nDONE=1;			//Start ADC Conversion
	while (GO_nDONE=1); //wait while ADC conversion is complete
	return ADRES;		//return ADC result
}	

//=============================================================================
//	                 			Delay Routine 4ms increment
//=============================================================================

void delay4ms(unsigned char d)
{
	clk=0;
	while (clk <d);
}	

//=============================================================================
//	                 		Key Press Handling Routine
//=============================================================================

void keyHandler()
{
	unsigned char tempMode;
	sec=0; //reset timeout
	tempMode=mode;	//save current mode/option
	
	
	 
		 if (keyFlag==1 & longKeyFlag==0) //short key press
		 { 
			
			if (mode <= 2)
			{	
				mode++ ;
				if (mode>2) mode=1;
			 }
			
			if (mode == 3) mode=1;
			if (mode == 4) mode=2;
		  } 
			
		 if (keyFlag==1 & longKeyFlag==1) //long key press
		 {
			 
			if (tempMode==1) mode=3;
			if (tempMode==2) mode=4;
			if (tempMode==3) mode=1;
			if (tempMode==4) mode=2;
			
			ledBuzzer=1;
			delay4ms(13);	//50ms delay
			ledBuzzer=0;
		 }	
		 
		 while (pushSwitch==0); //hold till key is released
		 keyFlag=0;
		 longKeyFlag=0;
		 keyRead=0;
	
}	

//=============================================================================
//	                 			Sleep Routine
//=============================================================================
void sleep()
	{
		modeDefault();

			unsigned char i;
	
	for (i=0;i<=1;i++)
		{
			ledBuzzer=1;
			delay4ms(13);	//50ms delay
			ledBuzzer=0;
			delay4ms(13);	//50ms delay
		}

		IOCIE=1;			//Interrupt on change enable
		IOCAP=0b00000000;	// INTERRUPT-ON-CHANGE PORTA POSITIVE EDGE REGISTER (pg 75 datasheet)
		IOCAN=0b00001010;	// INTERRUPT-ON-CHANGE PORTA pushSwitch RA3 and probeInput enabled (falling edge)
		IOCAF=0b00000000; 	//Clear IOC FLAG REGISTER 

		sleepFlag=1;

		SLEEP();	//microcontroller sleeps
		
		IOCIE=0;	//Interrupt on change disabled
		
		modeIndicator();
		modeSetup();
		
		delay4ms(120);	//500ms delay

		sleepFlag=0;
		sec=0;
			
		
		

	}//end sleep()

//=============================================================================
//                       Mode Indicator Beep Routine
//=============================================================================
void modeIndicator()
	{
	unsigned char i;
		modeDefault();
		
		ledBuzzer=0;
		delay4ms(50);	//200ms delay	
		
		for (i=0;i<mode;i++)
			{
				ledBuzzer=1;
				delay4ms(50);	//200ms delay
				ledBuzzer=0;
				delay4ms(100);	//400ms delay;
			}
	
	}

//=============================================================================
//                       Default Mode - Port initialization
//=============================================================================

void modeDefault()
{
	TRISA = 0b00000010;		//RA1 input (probeInput)
	ANSELA= 0b00000000; 	//No analog input (probeInput is digital input and pulled up externally)
	ADON=0;					//ADC module off
	chargePump=0;		// Turn off peripherals
	ledBuzzer=0;		
}
//=============================================================================
//                       Operation Mode - Port initialization
//=============================================================================

void modeSetup()
{

	if (mode==1 | mode==2 | mode==3)	//Continuity Tester and Diode mode 
		{
			TRISA = 0b00000010;		//RA1 input 
			ANSELA = 0b00000010; 	//AN1 analog input (probeInput)
			ADCON0 = 0b01000101;	//ADCS Fosc/32, select Ch AN1
			
			PWM1CON=0b00000000; //PWM Module OFF and Disable
			TMR2ON=0;			//Timer2 off
			
			chargePump=0; //charge pump off
			ledBuzzer=0;  //LED and Buzzer off
		}

	if (mode==4)	// LED Tester mode
		{
			TRISA =  0b00000100;		//RA2 input
			ANSELA = 0b00000100; 	//AN2 analog input
			ADCON0 = 0b01001001;	// ADCS Fosc/32, Ch AN2 
			//probeInput=0; //RA1 is output and ground	
			LATA1=0;
			chargePump=0; //charge pump off
						
			PWM1CON=0b11000000; // PWM Module ON and Enable
			TMR2ON=1;	//Timer2 on
			
		}

}




//=============================================================================
//                       Interrupt Service Routine
//=============================================================================
void interrupt isr()
{
	static unsigned char key=0, longKey=0, tick=0;
	
	//handle Interrupt on change  (falling edge)
	if (IOCIF==1) IOCAF = 0x00;	//clear the IOC Flag register
	
	//handle Timer2 interrupt
	if (TMR2IF) TMR2IF=0; //clear Timer2 interrupt Flag register
		
	//handle Timer0 overflow Interrupt tick = 4.1 ms, 1 sec = 244 ticks
	if (TMR0IF==1)
		{			
			tick++;
			clk++;
			if (tick>244)	
				{
				tick=0;
				sec++;
				 }
	
	//detect key press			
		if (pushSwitch)
			    { 
			      if (key != 0)  key--; // Will never go below zero
			     }
		  else
			    { 
				  if (key != 30 & keyRead!=1)  key++; // 123 ms keypress
			        
			    }
			    
			if (sleepFlag==1) //reset any key press just after wakeup
			{
				key=0;
				longKey=0;	
			}	    
			    
		    if (key >= 29 & keyRead!=1)		// 1168ms long key press
			    {
				    keyFlag = 1;
				    if (longKey != 255) longKey++;
				    if (longKey == 255) 
				     {
					     longKeyFlag=1;
					     keyRead=1;
					     longKey=0;  
					 	 key=0;
					    } 
					 if (pushSwitch) //key released
					 {
					 	keyRead=1;
					 	longKey=0;  
					 	key=0;	
					 }	
				  } 	
	
		TMR0IF = 0;	//clear Timer0 interrupt Flag register
	   }
		 
	
	

} //end ISR

//---------------------------------- EOF --------------------------------------