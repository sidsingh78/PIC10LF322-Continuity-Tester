//main.c
/*****************************************************************************************
 * Project:         Continuity Tester
 * Version:         V1.0
 * Date Created:    03/20/2018
 * Date Modified:   03/24/2018
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

bit diodeFlag;
unsigned short dutyCycle;


void main()		
{

	unsigned char adcResult;

	initHardware();
	

while(1)

{

	if (mode==1)		//Contuinity tester
		{
			
			modeIndicator();
			modeSetup();

			while (mode==1)

			{
				adcResult = readADC();	
				if (adcResult<=1) 
					{
						 ledBuzzer=1;	//Turn on buzzer
						 delay4ms(13);	//50ms delay
						 sec=0;			// reset timeout	
					}
							else ledBuzzer=0;	
					
					/*	
						if (adcResult>1 & adcResult<=4)
						{
							ledBuzzer=1;	//Turn on buzzer
							delay4ms(13);	//50ms delay
							ledBuzzer=0;	//Turn off buzzer
							delay4ms(50);	//50ms delay
						}*/
						
			if (keyRead) keyHandler();			
			if (sec>180) sleep();
			
			} //while (mode==1)
			
		}
		
		
else if (mode==2)
		{
			modeIndicator();
			modeSetup();
			
			diodeFlag=0;

			while (mode==2)

			{
						adcResult = readADC();
						
						if (adcResult>250) diodeFlag=0; 	//Open Circuit	- no beep
						
						if (adcResult>38 & adcResult<101 & diodeFlag==0) //Most silicon diodes - beep once
							{
							 ledBuzzer=1;	//Turn on buzzer
							 delay4ms(25);	//100ms delay
							 ledBuzzer=0;	//Turn off buzzer		
							 delay4ms(25);	//100ms delay
							 
							 diodeFlag=1;
							 sec=0;			// reset timeout
							}
						
						if (adcResult>14 & adcResult<21 & diodeFlag==0) 	//schottky diodes - beep twice
							{
							 ledBuzzer=1;	//Turn on buzzer
							 delay4ms(8);	//32ms delay
							 ledBuzzer=0;	//Turn off buzzer
							 delay4ms(25);	//100ms delay		
							 
							 ledBuzzer=1;	//Turn on buzzer
							 delay4ms(8);	//32ms delay
							 ledBuzzer=0;	//Turn off buzzer
							 delay4ms(25);	//100ms delay	
							 
							 sec=0;			// reset timeout
							 diodeFlag=1;
							}				
						
						if (adcResult<=1) //Shorted - continuous beep
							{
						 	ledBuzzer=1;	//Turn on buzzer
						 	delay4ms(13);	//50ms delay
						 	sec=0;			// reset timeout	
							}
							else ledBuzzer=0;		
			
				if (keyRead) keyHandler();					
				if (sec>180) sleep();		//3 min timeout
			//	if (sec>10) sleep();		
				} //while (mode==2)

		}		

else if (mode==3)	//reverse continuity tester
	{
		//	modeIndicator();
			modeSetup();

			while (mode==3)

			{
				adcResult = readADC();
						
						if (adcResult<=1) 
							{
							 ledBuzzer=0;	//Turn off buzzer 
							 }
							else ledBuzzer=1;	
							delay4ms(2);	//8ms delay
							
							ledBuzzer=0;	//turn off led & buzzer before reading ADC
				
				if (keyRead) keyHandler();			
				if (sec>240) sleep();	//4 min timeout	
			} //while (mode==3)
		}


		

	else if (mode==4)
		{
			dutyCycle=100;
			modeIndicator();
			modeSetup();
	
			
			while (mode==4)

			 {
						adcResult = readADC();	
							
						if (adcResult < 130) dutyCycle++;		//Regulate output
						if (adcResult > 130) dutyCycle--;
						
						if (dutyCycle > 200) dutyCycle=200;
						if (dutyCycle < 1) dutyCycle=1;
								
						PWM1PW=dutyCycle >> 2;
						PWM1PWL=dutyCycle << 6;
			
			if (keyRead) keyHandler();					
			if (sec>180) sleep();
			}//while (mode==4) 
		
		}


 }	//endless loop


}	//End of main()

//---------------------------------- EOF --------------------------------------