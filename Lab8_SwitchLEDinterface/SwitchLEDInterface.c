// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// ***** 2. Global Declarations Section *****
unsigned long SW1;	//Input from PE0
unsigned long Out;	//Output from PE1
int toggle;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void Delayms(unsigned long msec);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
	PortE_Init();
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    GPIO_PORTE_DATA_R |= 0x02;
		SW1 = GPIO_PORTE_DATA_R&0x01;
		toggle = 0;
		while(SW1){		
			Delayms(100);
			if(toggle){
				GPIO_PORTE_DATA_R |= 0x02;
				toggle = 0;
			}
			else{
				GPIO_PORTE_DATA_R &= ~0x02;
				toggle = 1;
			}
			SW1 = GPIO_PORTE_DATA_R&0x01;			
		}
  }
  
}

void PortE_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_AMSEL_R &= ~0x03; 		//Disable analog function
	GPIO_PORTE_PCTL_R &= ~0xFF;			//GPIO clear bit PCTL
	GPIO_PORTE_DIR_R |= 0x02;				//PE1 Output
	GPIO_PORTE_DIR_R &= ~0x01;			//PE0 Input
	GPIO_PORTE_AFSEL_R &= 0x00; 		//No alternate function
	GPIO_PORTE_PUR_R &= 0x00;				//Disable pullup
	GPIO_PORTE_DEN_R |= 0x03;				//Enable digtal pins PE 1-2
}

void Delayms(unsigned long msec){
// write this function
unsigned long i;
	while(msec > 0){
		i = (16000*25/30);
		while(i > 0){
			i = i -1;
		}
		msec = msec - 1;
	}
}
