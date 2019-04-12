// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Enable clock for PortB
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; 
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_AMSEL_R &= ~0x0F; // Clear AMSEL
	GPIO_PORTB_PCTL_R &= ~0x0000FFFF;  // Clear PCTL
	GPIO_PORTB_DIR_R |= 0x0F;  // PB0-3 out
	GPIO_PORTB_AFSEL_R &= ~0x0F; // Clear AFSEL
	GPIO_PORTB_DR8R_R |= 0x0F; // can drive up to 8mA out
	GPIO_PORTB_DEN_R |= 0x0F; // Enable digital
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = data;
}
