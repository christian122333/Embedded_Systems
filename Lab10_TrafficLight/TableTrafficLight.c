// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define SENSOR (*((volatile unsigned long *)0x4002401C))
#define CAR_LIGHT (*((volatile unsigned long *)0x400053FC))
#define WALK_LIGHT (*((volatile unsigned long *)0x400253FC))


#define GPIO_PORTF_DIR_R      (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R    (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DEN_R      (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R    (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R     (*((volatile unsigned long *)0x4002552C))

#define GPIO_PORTB_DIR_R      (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R    (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R      (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R    (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R     (*((volatile unsigned long *)0x4000552C))

#define GPIO_PORTE_DIR_R      (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R    (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R      (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R    (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R     (*((volatile unsigned long *)0x4002452C))
	
// System Clock
#define SYSCTL_RCGC2_R        (*((volatile unsigned long *)0x400FE108))
// SysTick Timer
#define NVIC_ST_CTRL_R        (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R      (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R     (*((volatile unsigned long *)0xE000E018))
 
	
struct State{
	unsigned long LEDOut;
	unsigned long WalkOut;
	unsigned long Time;
	unsigned long Next[8];
}; typedef const struct State StateType;

#define GoWest 0
#define WestYellow 1
#define GoSouth 2
#define SouthYellow 3
#define Walk 4
#define DontWalk 5
#define WalkOff 6
#define DontWalk2 7
#define WalkOff2 8 


#define RED 0x02
#define GREEN 0x08
#define OFF 0x00

StateType FSM[9] = {
	{0x0C,RED,50,{GoWest, GoWest, WestYellow, WestYellow, WestYellow, WestYellow, WestYellow, WestYellow}}, //GoWest 
	{0x14,RED,10,{GoSouth, GoSouth, GoSouth, GoSouth, Walk, Walk, Walk, GoSouth}},	//WestYellow
	
	{0x21,RED,50,{GoSouth, SouthYellow, GoSouth, SouthYellow, SouthYellow, SouthYellow, SouthYellow, SouthYellow}}, //GoSouth
	{0x22,RED,100,{GoWest, GoWest, GoWest, GoWest, Walk, GoWest, Walk,Walk}}, //SouthYellow
	
	{0x24,GREEN,50,{Walk, DontWalk, DontWalk, DontWalk, Walk, DontWalk, DontWalk, DontWalk}}, //Walk
	{0x24,RED,10,{WalkOff, WalkOff, WalkOff, WalkOff, WalkOff, WalkOff, WalkOff, WalkOff}}, //DontWalk
	{0x24,OFF,10,{DontWalk2, DontWalk2, DontWalk2, DontWalk2, DontWalk2, DontWalk2, DontWalk2, DontWalk2}}, //WalkOff
	{0x24,RED,10,{WalkOff2, WalkOff2, WalkOff2, WalkOff2, WalkOff2, WalkOff2, WalkOff2, WalkOff2}}, //DontWalk2
	{0x24,OFF,10,{Walk, GoWest, GoSouth, GoWest, Walk, GoWest, GoSouth, GoWest}}}; //WalkOff2

unsigned long CurrState;
unsigned long Input;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(void);
void Ports_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	SysTick_Init();
	Ports_Init();
	CurrState = GoWest;
  EnableInterrupts();
  while(1){
    CAR_LIGHT  = FSM[CurrState].LEDOut;  
		WALK_LIGHT = FSM[CurrState].WalkOut;
    SysTick_Wait10ms(FSM[CurrState].Time);
		Input = SENSOR;
    CurrState = FSM[CurrState].Next[Input];  
  }
}

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;        				// disable SysTick during set up
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

void Ports_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x32; // F B E 
	delay = SYSCTL_RCGC2_R; // No need to unlock
	
	GPIO_PORTE_AMSEL_R &= ~0x07; // Disable analog function on PE1-0
	GPIO_PORTE_PCTL_R &= ~0x000000FF; // Enable regular GPIO
	GPIO_PORTE_DIR_R &= ~0x07; // Inputs PE1-0
	GPIO_PORTE_AFSEL_R &= ~0x07; // Regular function on PE1-0
	GPIO_PORTE_DEN_R |= 0x07; // Enable digital on PE1-0
	
	GPIO_PORTB_AMSEL_R &= ~0x3F; // Disable analog function on PB5-0
	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // Enable regular GPIO
	GPIO_PORTB_DIR_R |= 0x3F; // Outputs on PB5-0
	GPIO_PORTB_AFSEL_R &= ~0x3F; // Regular function on PB5-0
	GPIO_PORTB_DEN_R |= 0x3F; // Enable digital on PB5-0

	GPIO_PORTF_AMSEL_R = 0x00; // Disable analog function on PF1 & PF3
	GPIO_PORTF_PCTL_R = 0x00; // Enable regular GPIO
	GPIO_PORTF_DIR_R |= 0x0A; // Outputs on PF1 & PF3
	GPIO_PORTF_AFSEL_R = 0x00; // Regular function on PF1 & PF3
	GPIO_PORTF_DEN_R |= 0x0A; // Enable digital on PF1 & PF3

}

void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}


void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}

