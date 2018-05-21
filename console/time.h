#ifndef TIME_H
#define TIME_H

#define PF1             (*((volatile uint32_t *)0x40025008))
#define PF2             (*((volatile uint32_t *)0x40025010))
		
#define START_HOUR 12
#define START_MINUTE 0 
#define MAX_ALARMS 3
#define ENABLE 1
#define DISABLE 0

//initialize systick to interrupt every 200ms
//initializae time variables
/***************** Time_Init ****************
	Initializes the current time, SysTick, and
	adds a default alarm
	Inputs: none
	Outputs: none
*/
void Time_Init(void);
/***************** Get_Update_Display_Bit ****************
	Gets the update display bit
	Inputs: none
	Outputs: update display bit
*/
uint8_t Get_Update_Display_Bit(void);
/***************** Get_Hour ****************
	Gets the current hour 
	Inputs: none
	Outputs: current hour
*/
uint32_t Get_Hour(void);
/***************** Get_Minute ****************
	Gets the current minutes
	Inputs: none
	Outputs: current minutes
*/
uint32_t Get_Minute(void);
/***************** Get_Second ****************
	Gets the current ms
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Second(void);
/***************** Get_Millisecond ****************
	Gets the current ms
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Millisecond(void);
/***************** Get_Epoch ****************
	Returns time since start of program in milliseconds
	Inputs: none
	Outputs: current ms
*/
uint32_t Get_Epoch(void);
/***************** Set_Time ****************
	Sets the current time
	Inputs: h - hour to set the current time
					m - minutes to set the current time
	Outputs: none
*/
void Set_Time(uint32_t h, uint32_t m);
/***************** Set_Timeout ****************
	Sets a timeout counter
	Inputs: numSeconds - number of seconds to set the timeout
	Outputs: none
*/
void Set_Timeout(uint32_t numSeconds);
/***************** Get_Timeout_Flag ****************
	Gets the timeout flag
	Inputs: none
	Outputs: the time out flag
*/
int32_t Get_Timeout_Flag(void);
/***************** Clear_Timeout_Flag ****************
	Clears the timeout flag
	Inputs: none
	Outputs: none
*/
void Clear_Timeout_Flag(void);
/***************** Reset_Time ****************
	Resets start of program time
	Inputs: none
	Outputs: current ms
*/
void Reset_Time(void);

#endif
