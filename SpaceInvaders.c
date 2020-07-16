// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "DAC.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0X20;
	volatile int wait =0;
	volatile int wait1 =0;
	volatile int wait2 =0;
	
	GPIO_PORTF_DIR_R &= 0XCF;
	GPIO_PORTF_DIR_R |= 0X04;
	GPIO_PORTF_DEN_R |= 0XFF;
	GPIO_PORTF_PUR_R = 0X11;

}
void PortE_Init(void){
	SYSCTL_RCGCGPIO_R |= 0X10;
	volatile int wait =0;
	volatile int wait1 =0;
	volatile int wait2 =0;
	
	GPIO_PORTE_DIR_R &= 0XCF;
	
	GPIO_PORTE_DEN_R |= 0XFF;
}


int32_t oldpos;
uint32_t Position;    // 32-bit fixed-point 0.001 cm
uint32_t enemypos;
int32_t ADCmail; // 0 to 4095
uint32_t ADCstatus; // 1 means new data
uint32_t color;
uint32_t bulletactive; 
uint32_t bullet; 
int32_t bulletpos;
int32_t shotpos;
int color1;
int enemy1status;
int enemy2status;
int enemy3status;
int enemy4status;
int enemy5status;
int enemy6status;
int enemycount;
int score;
int lang;
void SysTick_Init(void){
  
	NVIC_ST_CTRL_R &= 0;
	NVIC_ST_RELOAD_R = 8000000;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R  = 0X07;
	
	
}

void SysTick_Handler(void){
	NVIC_ST_RELOAD_R = 8000000;
  GPIO_PORTF_DATA_R ^= 0x04; // toggle PF2
	ADCmail = ADC_In();
	ADCstatus = 1;   
	score++;
	
	}

void draw_enemy(){
	if(enemy1status==0){ST7735_DrawBitmap(0, enemypos, SmallEnemy20pointB, 16,10);}
			if(enemy2status==0){ST7735_DrawBitmap(20,enemypos, SmallEnemy20pointB, 16,10);}
			if(enemy3status==0){ST7735_DrawBitmap(40,enemypos, SmallEnemy20pointB, 16,10);}
			if(enemy4status==0){ST7735_DrawBitmap(60,enemypos, SmallEnemy20pointB, 16,10);}
			if(enemy5status==0){ST7735_DrawBitmap(80,enemypos, SmallEnemy20pointB, 16,10);}
			if(enemy6status==0){ST7735_DrawBitmap(100,enemypos, SmallEnemy20pointB, 16,10);}
}

void game_won(void){
			if(enemycount==0){
				ST7735_FillScreen(0x0000);   // set screen to black
					ST7735_SetCursor(1, 1);
					ST7735_OutString("YOU HAVE");
				ST7735_SetCursor(1, 2);
				ST7735_OutString("SAVED EARTH");
					score = score-60;
				ST7735_SetCursor(1, 3);
				ST7735_OutString("Score:");
					LCD_OutDec(score);
				while(1){}
			}
}

void game_lost(void){
	if(enemypos==159){
					Sound_Killed();
					ST7735_FillScreen(0x0000);   // set screen to black
					ST7735_SetCursor(1, 1);
					if(lang==1){ // english
					ST7735_OutString("GAME OVER");
					ST7735_SetCursor(1, 2);
					ST7735_OutString("Nice try,");
					ST7735_SetCursor(1, 3);
					ST7735_OutString("Earthling!");
					ST7735_SetCursor(1, 4);
					score = score-((6-enemycount)*10);
					ST7735_OutString("Score:");
					LCD_OutDec(score);
					}
					else{ // spanish
					ST7735_OutString("JUEGO TERMINADO");
					ST7735_SetCursor(1, 2);
					ST7735_OutString("Buen Intento,");
					ST7735_SetCursor(1, 3);
					ST7735_OutString("Tierra!");
					ST7735_SetCursor(1, 4);
					score = score-((6-enemycount)*10);
					ST7735_OutString((char *)"Puntuac\xA2n:");
					LCD_OutDec(score);
					}
					
				while(1){}
			}
}

void player_move(void){
		int dist=7;
			if((ADCmail-oldpos)>2){
				Position+=dist;
				ST7735_DrawBitmap(Position, 157, PlayerShip1, 18,8); // player ship bottom
				ST7735_FillRect(Position-5,153,5,6,color);
				
			}
			else if(ADCmail-oldpos<-2){
				Position-=dist;
				ST7735_DrawBitmap(Position, 157, PlayerShip1, 18,8); // player ship bottom
				ST7735_FillRect(Position+18,153,10,6,color); 
				
			}
}

void bullet_move(void){
			//bullet
				if((bulletactive==0)&&((GPIO_PORTE_DATA_R&0x10)==0x10)){
				// this will create a new bullet once the old one is off the screen
				Sound_Shoot();
				shotpos = Position+8;
				ST7735_FillRect(shotpos,bulletpos,2,5,bullet);
				bulletactive=1;
				
			}
			if(bulletpos<=0){
				// this will reset the bullet so the player can shoot again
				ST7735_FillRect(shotpos,bulletpos+15,2,5,color);
				bulletactive=0;
				bulletpos=146;
				
			}
			
			else if((bulletpos>0)&&(bulletactive==1)){
							
				ST7735_FillRect(shotpos,bulletpos,2,5,bullet);
				ST7735_FillRect(shotpos,bulletpos+15,2,5,color);
				bulletpos-=15;
			}
			enemypos++;
			
		}

void enemy_die(void){
		//destroying enemies
			int location = bulletpos-enemypos;
			if((location<1)&&(location>-15)){
				 
				//statues of 0 means alive and 1 means dead
				
				if((shotpos>0)&&(shotpos<18)){
					if(enemy1status==0){enemycount--;}
					Sound_Explosion();
					enemy1status =1;
					ST7735_FillRect(0,enemypos-10,16,10,color);
					
				}
				
				if((shotpos>18)&&(shotpos<38)){
					if(enemy2status==0){enemycount--;}
					Sound_Explosion();
					enemy2status =1;
					ST7735_FillRect(20,enemypos-10,16,10,color);
					
				}
				
				if((shotpos>38)&&(shotpos<58)){
					if(enemy3status==0){enemycount--;}
					Sound_Explosion();
					enemy3status =1;
					ST7735_FillRect(40,enemypos-10,16,10,color);
					
				}
				
				if((shotpos>58)&&(shotpos<78)){
					if(enemy4status==0){enemycount--;}
					Sound_Explosion();
					enemy4status =1;
					ST7735_FillRect(60,enemypos-10,16,10,color);
					
				}
				
				if((shotpos>78)&&(shotpos<98)){
					if(enemy5status==0){enemycount--;}
					Sound_Explosion();
					enemy5status =1;
					ST7735_FillRect(80,enemypos-10,16,10,color);
					
				}
				
				if((shotpos>98)&&(shotpos<118)){
					if(enemy6status==0){enemycount--;}
					Sound_Explosion();
					enemy6status =1;
					ST7735_FillRect(100,enemypos-10,16,10,color);
					
				}
					
				}	
}
		
void pause_play(void){
	while((GPIO_PORTE_DATA_R&0x20)==0x20){}
}	
		
		

int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  //ST7735_DrawBitmap(52, 125, PlayerShip1, 18,8); // player ship bottom	
	SysTick_Init();
	PortF_Init();
	PortE_Init();
	Sound_Init();
	ADC_Init();  
  EnableInterrupts();
	// turn on ADC, set channel to 5
	ADC0_SAC_R = 4;   // 16-point averaging change this to have faster times 1 is fastest but less accurate
	ADCmail=ADC_In();
	
	ADCstatus=1;
	//ST7735_InitR(INITR_REDTAB);
	 Position=52; // the postion on the player.
	 oldpos = ADC_In(); // this will check if to go left or right
	 enemypos=20; // the enemies y axis
	 color = ST7735_Color565(0,0,0);
	 color1 = ST7735_Color565(255,0,0);
	 bulletactive=0;
	 bullet = ST7735_Color565(0,0,255);
	 bulletpos = 146;
	 enemy1status=0;
	 enemy2status=0;
	 enemy3status=0;
	 enemy4status=0;
	 enemy5status=0;
	 enemy6status=0;
	 enemycount=6;
	 score =0;
	 lang=0;
	 
	 ST7735_FillScreen(0x0000);   // set screen to black
					ST7735_SetCursor(1, 1);
					ST7735_OutString("2 For");
				ST7735_SetCursor(1, 2);
				ST7735_OutString("English");
				ST7735_SetCursor(1, 3);
				ST7735_OutString("1 Para La");
				ST7735_SetCursor(1, 4);
				ST7735_OutString((char *)"Espa\xA4ol");
				
				while(((GPIO_PORTE_DATA_R&0x10)!=0x10)&&((GPIO_PORTE_DATA_R&0x20)!=0x20)){
				if((GPIO_PORTE_DATA_R&0x10)==0x10){lang=0;}// 0 means english
				if((GPIO_PORTE_DATA_R&0x20)==0x20){lang=1;}// 1 means spanish
				}
				if((GPIO_PORTE_DATA_R&0x10)==0x10){lang=0;}// 0 means english
				if((GPIO_PORTE_DATA_R&0x20)==0x20){lang=1;}// 1 means spanish
				ST7735_FillScreen(0x0000);
				ST7735_DrawBitmap(52, 157, PlayerShip1, 18,8); // player ship bottom
					
  while(1){
		
		if(ADCstatus==1){
			ADCstatus=0;
			draw_enemy();
			game_won();		
			game_lost();
			player_move();
			bullet_move();
			enemy_die();
			pause_play();
			}
			oldpos = ADCmail;
		}
		
	}			
  

//----------------------------------------------------------------------------------

// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count)
{
		uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
