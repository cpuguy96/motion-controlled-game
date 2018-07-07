//***********************  buttons.c  ***********************
// Program written by: Chimezie Iwuanyanwu & Cole Thompson
//

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "buttons.h"
#include "Music.h"
#include "Timer3.h"
#include "lcd.h"

/***************** GPIO_Arm ****************
	Turns on port E for interrupts
	Inputs: none
	Outputs: none
*/
void GPIO_Arm(void){
	GPIO_PORTE_ICR_R = 0x07;      				// clear flag0
  GPIO_PORTE_IM_R |= 0x07;      				// arm interrupt on PE2-0 
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000040; 					// priority 2
  NVIC_EN0_R |= 0x010;      						// enable interrupt 4 in NVIC  book page 269
}

/***************** LeftButton ****************
	Checks if left button was pressed
	Inputs: data - port E data register data
	Outputs: 1 if button is pressed
*/
uint8_t LeftButton(uint32_t data) {
	return (data >> 2) & 0x01;
}
/***************** MiddlePressed ****************
	Checks if middle button was pressed
	Inputs: data - port E data register data
	Outputs: 1 if button is pressed
*/
uint8_t MiddleButton(uint32_t data){
	return (data >> 1) & 0x01;
}
/***************** RightPressed ****************
	Checks if right button was pressed
	Inputs: data - port E data register data
	Outputs: 1 if button is pressed
*/
uint8_t RightButton(uint32_t data){
	return data & 0x01;
}

/***************** GPIOPortE_Handler ****************
	Handler for button interrupts
	Inputs: none
	Outputs: none
*/
void GPIOPortE_Handler(void){
	GPIO_PORTE_IM_R &= ~0x07;
	uint32_t data = GPIO_PORTE_RIS_R & 0x07;
	if(LeftButton(data)){
		Play_Button();
	}
	else if(MiddleButton(data)){
		Pause_Button();
	}
	else if(RightButton(data)){
		Reset_Button();
	}
	Timer3_Arm(&GPIO_Arm);
}

/***************** Buttons_Init ****************
	Initializes Port E as button inputs
	Inputs: none
	Outputs: none
*/
void Buttons_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x10;            // activate port E
	while ((SYSCTL_RCGCGPIO_R & 0x10) == 0x00);  //allow time to finish activating
	GPIO_PORTE_DIR_R &= ~0x07;            // make PE2-0 in for buttons
  GPIO_PORTE_AFSEL_R &= ~0x07;          // disable alt funct on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;             // enable digital I/O PE2-0
  GPIO_PORTE_AMSEL_R = 0;               // disable analog functionality on PE
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; 		// configure PE2-0 as GPIO

	//Interrupt Enable for Stop Button PE2-0
	GPIO_PORTE_IS_R &= ~0x07;     				// PE2-0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x07;     				// PE2-0 is not both edges
	GPIO_PORTE_IEV_R &= ~0x07;
	GPIO_Arm();
}
