//***********************  lcd.c  ***********************
// Program written by: Chimezie Iwuanyanwu & Cole Thompson
//
#include <stdint.h>
#include <stdio.h>
#include "lcd_constants.h"
#include "lcd.h"
#include "time.h"
#include "ST7735.h"

static uint8_t CurrentScreen;						// current screeen

/***************** LCD_Init ****************
	Initializes the LCD and displays the inital screen
	Inputs: none
	Outputs: none
*/
void LCD_Init(void){
	ST7735_InitR(INITR_REDTAB);
	CurrentScreen = INIT_SCREEN;
}
/***************** Get_Current_Screen *****************
	Returns the current screen
	Inputs: none
	Outputs: current screen
*/
uint8_t Get_Current_Screen(void){
	return CurrentScreen;
}
/***************** Set_Current_Screen *****************
	Returns the current screen
	Inputs: screen - screen number
	Outputs: none
*/
void Set_Current_Screen(uint8_t screen){
	CurrentScreen = screen;
}
/***************** Play_Button *****************
	Sets current screen to the play screen
	Inputs: none
	Outputs: none
*/
void Play_Button(void){
	CurrentScreen = GAME;
}
/***************** Pause_Button *****************
	Sets current screen to the pause screen
	Inputs: none
	Outputs: none
*/
void Pause_Button(void){
	CurrentScreen = PAUSE;
}
/***************** Reset_Button *****************
	Sets current screen to the reset screen
	Inputs: none
	Outputs: none
*/
void Reset_Button(void){
	CurrentScreen = RESET;
}

