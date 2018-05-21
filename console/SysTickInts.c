// SysTickInts.c
// Runs on LM4F120/TM4C123
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano
// October 11, 2012

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

   Program 5.12, section 5.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTickInts.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void (*PeriodicTaskSysTick)(void);   // user function

// **************SysTick_Init*********************
// Initialize SysTick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns (assuming 50 MHz clock)
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(void(*task)(void), uint32_t period){long sr;
  sr = StartCritical();
	PeriodicTaskSysTick = task;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
  EndCritical(sr);
}


void SysTick_Handler(void) {
	(*PeriodicTaskSysTick)(); 
}
/***************** SysTick_Reset_Per ****************
	Changes the BPM
	Inputs: period - new BPM in clock cycles
	Outputs: none
*/
void SysTick_Reset_Per(uint32_t period) {
	Pause_SysTick();
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R = 0;
	Play_SysTick();
}
/***************** Pause_SysTick ****************
	Stops SysTick
	Inputs: none
	Outputs: none
*/
void Pause_SysTick(void){
	NVIC_ST_CTRL_R = 0;
}
/***************** Play_SysTick ****************
	Resumes SysTick
	Inputs: none
	Outputs: none
*/
void Play_SysTick(void){
	NVIC_ST_CTRL_R = 0x07;
}
