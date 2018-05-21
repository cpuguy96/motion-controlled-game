// SysTickInts.h
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

#ifndef __SYSTICKINTS_H__ // do not include more than once
#define __SYSTICKINTS_H__

// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns (assuming 50 MHz clock)
//        Maximum is 2^24-1
//        Minimum is determined by lenght of ISR
// Output: none
void SysTick_Init(void(*task)(void), uint32_t period);
/***************** SysTick_Reset_Per ****************
	Changes the BPM
	Inputs: period - new BPM in clock cycles
	Outputs: none
*/
void SysTick_Reset_Per(uint32_t period);
/***************** Pause_SysTick ****************
	Stops SysTick
	Inputs: none
	Outputs: none
*/
void Pause_SysTick(void);
/***************** Play_SysTick ****************
	Resumes SysTick
	Inputs: none
	Outputs: none
*/
void Play_SysTick(void);

#endif // __SYSTICKINTS_H__
