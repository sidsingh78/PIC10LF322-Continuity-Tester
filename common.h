//common.h
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

#ifndef __COMMON_H
#define __COMMON_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 16000000		//16MHz Clock
#endif

extern bit keyRead;
extern unsigned char sec,mode,clk;


//port definitions
#define chargePump LATA0
#define probeInput PORTA1
#define ledBuzzer  PORTA2
#define pushSwitch PORTA3

//functions
void delay4ms(unsigned char);
void initHardware();
unsigned char readADC();
void sleep();
void modeIndicator();
void modeSetup();
void modeDefault();
void keyHandler();

#endif

//---------------------------------- EOF --------------------------------------