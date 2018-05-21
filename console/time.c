#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "time.h"
#include "Timer2.h"

//keep track of time
static uint32_t HalfDays, Hour, Minute, Second, Millisecond, Epoch;

//clock set to interrupt every 200 ms
static uint32_t InterruptCount;

//update bit 
//1 = Minute changed since last call to Get_Minute(), 0 otherwise
static uint8_t UpdateDisplay;

//screen timeout flag for short time outs. TimeoutFlag = -1 if disabled
static int32_t TimeoutMin, TimeoutSecond, TimeoutFlag;

//HELPER FUNCTIONS
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

/***************** Get_Hour ****************
	Gets the current hour 
	Inputs: none
	Outputs: current hour
*/
uint32_t Get_Hour(void) {
	return Hour;
}
/***************** Get_Minute ****************
	Gets the current minutes
	Inputs: none
	Outputs: current minutes
*/
uint32_t Get_Minute(void) {
	long sr;
  sr = StartCritical();
	UpdateDisplay = 0;
	EndCritical(sr);
	return Minute;
}
/***************** Get_Second ****************
	Gets the current ms
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Second(void) {
	return Second;
}
/***************** Get_Millisecond ****************
	Gets the current ms
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Millisecond(void) {
	return Millisecond;
}
/***************** Get_Epoch ****************
	Returns time since start of program in milliseconds
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Epoch(void) {
	return Epoch;
}

/***************** Reset_Time ****************
	Resets start of program time
	Inputs: none
	Outputs: current ms
*/
void Reset_Time(void){
	Epoch = 0;
	Hour = 0;
	Minute = 0;
	Second = 0;
	HalfDays = 0;
}
/***************** Set_Time ****************
	Sets the current time
	Inputs: h - hour to set the current time
					m - minutes to set the current time
	Outputs: none
*/
void Set_Time(uint32_t h, uint32_t m) {
	long sr;
  sr = StartCritical();
	
	Hour = h;
	Minute = m;
	Second = 0;
	HalfDays = 0;
	InterruptCount = 0;
	UpdateDisplay = 1;
	
	EndCritical(sr);
}

//Get "update display bit" to determine if Second has changes since last update
/***************** Get_Update_Display_Bit ****************
	Gets the update display bit
	Inputs: none
	Outputs: update display bit
*/
uint8_t Get_Update_Display_Bit(void) {
	return UpdateDisplay;
}
//Timeouts (one Minute max)
/***************** Set_Timeout ****************
	Sets a timeout counter
	Inputs: numSeconds - number of seconds to set the timeout
	Outputs: none
*/
void Set_Timeout(uint32_t numSeconds) {
	if (numSeconds <= 60 && numSeconds > 0) {
		long sr;
		sr = StartCritical();
		TimeoutFlag = 0;
		TimeoutSecond = numSeconds + Second;
		TimeoutMin = Minute;
		if (TimeoutSecond >= 60) {
			TimeoutSecond -= 60;
			TimeoutMin += 1;
		}
		EndCritical(sr);
	}
}
/***************** Get_Timeout_Flag ****************
	Gets the timeout flag
	Inputs: none
	Outputs: the time out flag
*/
int32_t Get_Timeout_Flag(void) {
	return TimeoutFlag;
}
/***************** Clear_Timeout_Flag ****************
	Clears the timeout flag
	Inputs: none
	Outputs: none
*/
void Clear_Timeout_Flag(void) {
		if (TimeoutFlag == 1) {
			TimeoutFlag = -1;
		}
}
/***************** CheckForTimeout ****************
	Checks for a timeout. Sets the timeout flag if there is
	Inputs: none
	Outputs: none
*/
void CheckForTimeout(void) {
	if ((TimeoutFlag == 0) && (TimeoutMin == Minute) && (TimeoutSecond == Second)) {
		TimeoutFlag = 1;
	}
}
/***************** Heartbeat ****************
	Toggles the heartbeat
	Inputs: none
	Outputs: none
*/
void Heartbeat(void){
	PF1 ^= 0x02;
}
// Interrupt service routine
// Executed every 12.5ns*(period)
/***************** SysTick_Handler ****************
	Updates the current time every 200ms and checks for timeout
	Inputs: none
	Outputs: none
*/
void TimeUpdate(void){
	Millisecond++;
	Epoch ++;
	if (Millisecond == 1000) {
		Millisecond = 0;
		Second++;
		if (Second == 60) {
			Second = 0;
			Minute++;
			UpdateDisplay = 1;
			PF2 ^= 0x04;		//timing lcd update time
			PF2 ^= 0x04;
			if (Minute == 60) {
				Minute = 0;
				Hour++;
				if (Hour == 13) {
					Hour = 1;
					HalfDays++;		//might as well
				}
			}
		}
		CheckForTimeout();
	}
	InterruptCount++;
	Heartbeat();
}


//initialize systick to interrupt every 200ms
//initializae time variables
//Systick max interrupt time = (2^24-1) * 12.5 * 10^-9 = 0.2097151875 seconds
/***************** Time_Init ****************
	Initializes the current time and SysTick
	Inputs: none
	Outputs: none
*/
void Time_Init(void) {
	Hour = START_HOUR;
	Minute = START_MINUTE;
	Second = 0;
	Epoch = 0;						// time since start of program in milliseconds
	Millisecond = 0;
	HalfDays = 0;
	InterruptCount = 0;
	UpdateDisplay = 1;
	TimeoutMin = 0;
	TimeoutSecond = 0;
	TimeoutFlag = -1;
	
	Timer2_Init(&TimeUpdate, 80000);		//1 ms
}

