// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

const unsigned char SineWave[64] = {5,5,5,5,4,3,2,1,0,1,2,
																		4,7,9,10,11,12,11,10,8,6,3,
																		2,2,4,6,9,12,14,15,14,12,
																		10,8,7,6,6,5,5,5,6,7,
																		7,8,8,8,7,6,5,3,3,2,
																		3,3,3,3,3,3,4,4,5,5,5,5};


unsigned char Index=0; 
unsigned long off;

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init(); // Port B is DAC
	NVIC_ST_CTRL_R = 0; // Disable SysTick during setup
	NVIC_ST_CURRENT_R = 0; // Any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // Priority 1
	NVIC_ST_CTRL_R = 0x0007; // Enable, core clock, and interrupts
  
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	//KEY C 9,555 Hz
	//KEY D 8,513 Hz
	//KEY E 7,584 Hz
	//KEY G 6,377 Hz
	off = 0x00;
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period-1;  // Reload value
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x0007;
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	off = 0x01;
	DAC_Out(0);
	Index = 0;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	if(!off){
		Index = (Index + 1) & 0x3F;
		DAC_Out(SineWave[Index]);
	}
}
