
#ifndef __lcd_h
#define __lcd_h

enum SCREENS{RESET, PAUSE, GAME, END, NUM_SCREENS};
enum TIME_VALUES{HOURS, MINUTES, SECONDS};

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
#define ALARM_WIDTH 50
#define DIGIT_PADDING 3
#define MIN_DIGIT_WIDTH 3	
#define INIT_SCREEN GAME

#define CENTER_CONT(x, y) ((x - y) / 2)
#define CENTER_SCREEN_X(x) (64 - (x/2))
#define CENTER_SCREEN_Y(x) (80 + (x/2))
#define FLOAT_R(x) (SCREEN_WIDTH - x)
#define FLOAT_BOT(x) (SCREEN_HEIGHT - x)
#define DIGIT_WIDTH(x) ((x - DIGIT_PADDING * 4) * 2) / 9

/***************** LCD_Init ****************
	Initializes the LCD and displays the inital screen
	Inputs: none
	Outputs: none
*/
void LCD_Init(void);

/***************** Get_Current_Screen *****************
	Returns the current screen
	Inputs: none
	Outputs: current screen
*/
uint8_t Get_Current_Screen(void);
/***************** Set_Current_Screen *****************
	Returns the current screen
	Inputs: screen - screen number
	Outputs: none
*/
void Set_Current_Screen(uint8_t screen);
/***************** Play_Button *****************
	Sets current screen to the play screen
	Inputs: none
	Outputs: none
*/
void Play_Button(void);
/***************** Pause_Button *****************
	Sets current screen to the pause screen
	Inputs: none
	Outputs: none
*/
void Pause_Button(void);
/***************** Reset_Button *****************
	Sets current screen to the reset screen
	Inputs: none
	Outputs: none
*/
void Reset_Button(void);
#endif
