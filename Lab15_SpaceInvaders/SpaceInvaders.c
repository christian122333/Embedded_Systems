// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "Sound.h"
#include "Player.h"
#include "Enemy.h"
#include "Images.h"
#include "TExaS.h"
#include <stdlib.h>

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))
	
enum GAME_STATE{GAME_START,GAME_OVER,GAME_WON};

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
int Collision(unsigned long x1, unsigned long y1, unsigned long w1, unsigned long h1, 
	unsigned long x2, unsigned long y2, unsigned long w2, unsigned long h2);
unsigned long TimerCount;
unsigned long Semaphore;


unsigned long ADCdata;

enum GAME_STATE state;
int Num_enemies = 8;
struct Enemy enemy[4];
struct Enemy enemy2[4];
struct Enemy_laser lasers[10];
struct Enemy_laser lasers2[10];
struct Player player;
struct Bunker bunker;
struct Missile Player_missile[5];

void Port_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x13; //Enable clock for PortA, B, and E
	delay = SYSCTL_RCGC2_R;
	//Port E
	GPIO_PORTE_DIR_R &= ~0x07; //PE0,1 input
	GPIO_PORTE_DEN_R |= 0x03; //Enable digital for PE0,1
	GPIO_PORTE_DEN_R &= ~0x04; //Disable digital for PE2
	GPIO_PORTE_AFSEL_R &= ~0x03; //Disable Alternate function for PE0,1
	GPIO_PORTE_AFSEL_R |= 0x04; //Enable Alternate function for PE2
	GPIO_PORTE_AMSEL_R &= ~0x03; //Disable Analog for PE0,1
	GPIO_PORTE_AMSEL_R |= 0x04; //Enable Analog for PE2
	GPIO_PORTE_PCTL_R &= ~0xFF; //Configure PE0,1 as GPIO
	GPIO_PORTE_IS_R &= ~0x03; //PE0,1 edge-sensitive
	GPIO_PORTE_IBE_R &= ~0x03; //PE0,1 not both edges 
	GPIO_PORTE_IEV_R |= 0x03; //PE0,1 rising edge interrupt
	GPIO_PORTE_ICR_R = 0x03; //Clear flag
	GPIO_PORTE_IM_R |= 0x03; //Arm interrupt PE0,1
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000000; //Priority 0
	NVIC_EN0_R = 0x10; //Enable interrupt 4 in NVIC
	//Port A
	GPIO_PORTA_DIR_R |= 0xEC; //PA2,3,5,6,7 output;
	GPIO_PORTA_DEN_R |= 0xEC; //Enable digital for PA2,3,5,6,7
	GPIO_PORTA_AFSEL_R &= ~0xEC; //Disable Alternate function for PA2,3,5,6,7
	GPIO_PORTA_AMSEL_R &= ~0xEC; //Disable Analog for PA2,3,5,6,7
	//ADC
	SYSCTL_RCGC0_R |= 0x10000; //Activate
	delay = SYSCTL_RCGC2_R;
	SYSCTL_RCGC0_R &= ~0x300; //Configure for 125k
	ADC0_SSPRI_R = 0x0123; //Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x08; //Disable sampler sequencer 3
	ADC0_EMUX_R &= ~0xF000; //Sequence 3 is software trigger
	ADC0_SSMUX3_R &= ~0x0F; //Clear SS3 field
	ADC0_SSMUX3_R += 1; //Set channel AIN1 (PE2)
	ADC0_SSCTL3_R = 0x06; //No TS0,D0 Yes IE0,END0
	ADC0_ACTSS_R |= 0x08; //Enable sampler sequencer 3
	//SysTick Init
	NVIC_ST_CTRL_R = 0; //Disable SysTick during setup
	NVIC_ST_CURRENT_R = 0; //Any write to current clears it
	NVIC_ST_RELOAD_R = 2666667 - 1; // 30Hz
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x60000000; //Set priority 3
	NVIC_ST_CTRL_R = 0x07; // Enable core,clock, and interrupts
	EnableInterrupts();
}

unsigned long ADC0_In(void){
	unsigned long result;
	ADC0_PSSI_R = 0x08; //Initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; //Wait for conversion to be done
	result = ADC0_SSFIFO3_R&0xFFF; //Read result
	ADC0_ISC_R = 0x08; //Acknowledge completion
	return result;
}
void SysTick_Handler(void){
		ADCdata = (ADC0_In() / 62);
	  Player_move(&player, ADCdata);
}

/*If input received from PortE, then the player will fire*/
void GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x03;
	Player_shoot(Player_missile, &player, LENGTH(Player_missile));
	Sound_Shoot();
}

void Draw(){
	int i;
	int j;
	Nokia5110_ClearBuffer();
	
	/*Draw the bunker on the buffer*/
	if(bunker.life > 7)
		Nokia5110_PrintBMP(bunker.x, bunker.y, bunker.bunker0_image, 0);
	else if(bunker.life > 4)
		Nokia5110_PrintBMP(bunker.x, bunker.y, bunker.bunker1_image, 0);
	else if(bunker.life > 1)
		Nokia5110_PrintBMP(bunker.x, bunker.y, bunker.bunker2_image, 0);
	else
		Nokia5110_PrintBMP(bunker.x, bunker.y, bunker.bunker3_image, 0);
	
	/*Draw the player on the buffer*/
	if(player.life > 0)
		Nokia5110_PrintBMP(player.x , player.y, player.player_image, 0);
	else if(player.explosion_frame > 1){
		Nokia5110_PrintBMP(player.x , player.y, player.explosion0_image, 0);
		Sound_Explosion();
		player.explosion_frame -= 1;
	}
	else if(player.explosion_frame > 0){
		Nokia5110_PrintBMP(player.x , player.y, player.explosion1_image, 0);
		player.explosion_frame -= 1;
		state = GAME_OVER;
	}
	
	/*Draw the enemies on the buffer*/
	for(i=0; i<LENGTH(enemy); i++){
		if(enemy[i].life > 0){
			Nokia5110_PrintBMP(enemy[i].x , enemy[i].y, enemy[i].enemy_image, 0);
		}
		else{
			if(enemy[i].explosion_frame == 2){
				Nokia5110_PrintBMP(enemy[i].x , enemy[i].y, enemy[i].explosion0_image, 0);
				enemy[i].explosion_frame -= 1;
			}
		else if(enemy[i].explosion_frame == 1){
				Nokia5110_PrintBMP(enemy[i].x , enemy[i].y, enemy[i].explosion1_image, 0);
				enemy[i].explosion_frame -= 1;
			}
		}
	}
	
	for(i=0; i<LENGTH(enemy2); i++){
		if(enemy2[i].life > 0){
			Nokia5110_PrintBMP(enemy2[i].x , enemy2[i].y, enemy2[i].enemy_image, 0);
		}
		else{
			if(enemy2[i].explosion_frame == 2){
				Nokia5110_PrintBMP(enemy2[i].x , enemy2[i].y, enemy2[i].explosion0_image, 0);
				enemy2[i].explosion_frame -= 1;
			}
		else if(enemy2[i].explosion_frame == 1){
				Nokia5110_PrintBMP(enemy2[i].x , enemy2[i].y, enemy2[i].explosion1_image, 0);
				enemy2[i].explosion_frame -= 1;
			}
		}
	}
	
	/*Draw the player weapons on the buffer*/
	for(i=0; i<LENGTH(Player_missile); i++){
		if(Player_missile[i].life > 0){
			for(j=0; j<LENGTH(enemy); j++){
				if(Collision(Player_missile[i].x, Player_missile[i].y, Player_missile[i].width, Player_missile[i].height,
					enemy[j].x, enemy[j].y, enemy[j].width, enemy[j].height) == 1){
						Player_missile[i].life = 0;
						enemy[j].life = 0;
						Sound_Killed();
						Num_enemies--;
						if(Num_enemies < 1)
							state = GAME_WON;
					}
			}
			if(Player_missile[i].y < 8)
				Player_missile[i].life = 0;
			else
				Nokia5110_PrintBMP(Player_missile[i].x , Player_missile[i].y, Player_missile[i].image, 0);
		}
	}
	
	/*Draw the enemy weapons on the buffer*/
	for(i=0; i<LENGTH(lasers); i++){
		if(lasers[i].life > 0){
			/*Check collision with the player struct for each enemy laser*/
			if(Collision(lasers[i].x, lasers[i].y, lasers[i].width, lasers[i].height,
				player.x, player.y, player.width, player.height) == 1){
					lasers[i].life = 0;
					player.life = 0;
					Sound_Explosion();
			}
			/*Check collision with the bunker struct for each enemy laser*/
			if(bunker.life > 0){
				if(Collision(lasers[i].x, lasers[i].y, lasers[i].width, lasers[i].height,
					bunker.x, bunker.y, bunker.width, bunker.height) == 1){
						bunker.life--;
						lasers[i].life = 0;
				}
			}
			if(lasers[i].y > 40)
				lasers[i].life = 0;
			else
				Nokia5110_PrintBMP(lasers[i].x , lasers[i].y, lasers[i].laser_image, 0);
		}
	}
  Nokia5110_DisplayBuffer(); 
}

int main(void){
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
	Port_Init();
	Sound_Init();
  Nokia5110_Init();
	state = GAME_START;
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer(); // draw buffer
	Enemy_init(enemy,10,RIGHT,SmallEnemy20PointA,LENGTH(enemy));
	Enemy_init(enemy2,20,LEFT,SmallEnemy10PointA,LENGTH(enemy2));
	Enemy_laser_init(lasers,LENGTH(lasers));
	Enemy_laser_init(lasers2,LENGTH(lasers2));
	Player_init(&player, &bunker, Player_missile,LENGTH(Player_missile));
	Draw();
  Delay100ms(5);              // delay 5 sec at 50 MHz
	while(1){
		if(state == GAME_OVER){
			Nokia5110_Clear();
			Nokia5110_SetCursor(1, 1);
			Nokia5110_OutString("GAME OVER");
		}else if(state == GAME_WON){
			Nokia5110_Clear();
			Nokia5110_SetCursor(1, 1);
			Nokia5110_OutString("GAME WON");
		}
		else{
			Enemy_move(enemy, lasers, LENGTH(enemy), LENGTH(lasers));
			Enemy_move(enemy2, lasers2, LENGTH(enemy2), LENGTH(lasers2));
			Missile_move(Player_missile,LENGTH(Player_missile));
			Draw();
		}
		Delay100ms(10); 
  }
	
}

int Collision(unsigned long x1, unsigned long y1, unsigned long w1, unsigned long h1, 
	unsigned long x2, unsigned long y2, unsigned long w2, unsigned long h2){
		if(x1>(x2+w2) || x2>(x1+w1)) return 0;
		if(y1>(y2+h2) || y2>(y1+h1)) return 0;
		return 1;
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
