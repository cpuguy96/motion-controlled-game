//***********************  main.c  ***********************
// Program written by: Chimezie Iwuanyanwu & Cole Thompson
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../inc/tm4c123gh6pm.h"
#include "pll.h"
#include "esp8266.h"				
#include "LED.h"						
#include "ST7735.h"					
#include "buttons.h"				
#include "gamemodel.h"
#include "time.h"						
#include "Packet.h"
#include "lcd.h"
#include "random.h"
#include "gameloop.h"


// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


//function declarations
void Init(void);
void GameInit(void);
void ShowConnectionStatus(void);
void DisplayEndScreen(void);


//game objects
static uint32_t ConnectionStatusTime;

static uint8_t CurrentScreenChange;

static int8_t WinningPlayer;



int main(void){
	Init();
	uint8_t prevScreen = CurrentScreen;
	
	while(1){
		uint8_t index;
		for(index = 0; index < NUM_PLAYERS; index++){
			if(World.players[index].stats[HEALTH] <= 0){
				WinningPlayer = NUM_PLAYERS - index - 1;
			}
		}
		
		CurrentScreen = Get_Current_Screen();
		if(CurrentScreen != prevScreen){
			CurrentScreenChange = TRUE;
		}
		prevScreen = CurrentScreen;
		for(index = 0; index < NUM_PLAYERS; index++){
			if(Get_Epoch() - Get_Last_Update_Time(index) > 2000){
				if(World.players[index].connection_status == TRUE){
					World.players[index].connection_status_change = TRUE;
				}
				World.players[index].connection_status = FALSE;
			}
			else{
				if(World.players[index].connection_status == FALSE){
					World.players[index].connection_status_change = TRUE;
				}
				World.players[index].connection_status = TRUE;
			}
		}
		if(WinningPlayer != -1){
			CurrentScreen = END;
			Set_Current_Screen(END);
		}
		//numbers smaller than 50 start to produce pixel diffs of 0
		switch(CurrentScreen){
			case GAME:
				if (GetMsDiff(CurrentGameTime, Get_Epoch()) > 50) {// might need to set loop time at beginning and call that instead
					CurrentGameTime = Get_Epoch();
					UpdateModel();
					SendGameData(FALSE);
				}
				ShowConnectionStatus();
				break;
			case PAUSE:
				CurrentGameTime = Get_Epoch();
				SendGameData(FALSE);
				TIMER2_CTL_R = 0x00000000;	// pauses time
				while(Get_Current_Screen() == PAUSE){
					for(index = 0; index < NUM_PLAYERS; index++){
						if(Get_Epoch() - Get_Last_Update_Time(index) > 2000){
							if(World.players[index].connection_status == TRUE){
								World.players[index].connection_status_change = TRUE;
							}
							World.players[index].connection_status = FALSE;
						}
						else{
							if(World.players[index].connection_status == FALSE){
								World.players[index].connection_status_change = TRUE;
							}
							World.players[index].connection_status = TRUE;
						}
					}
					
					ShowConnectionStatus();
				}
				TIMER2_CTL_R = 0x00000001;	// resumes time
				break;
			case RESET:
				Reset_Time();							// sets epoch back to 0
				GameInit();
				Play_Button();
				break;
			case END:
				CurrentGameTime = Get_Epoch();
				SendGameData(FALSE);
				DisplayEndScreen();
				while(Get_Current_Screen() == END){}
				Reset_Time();							// sets epoch back to 0
				GameInit();
				Play_Button();
				break;
		}
	}
}

/***************** ShowConnectionStatus ****************
	Displays the connection status to the LCD
	Inputs: none
	Outputs: none
*/
void ShowConnectionStatus(void){
	uint8_t index;
	uint8_t player1change = World.players[0].connection_status_change;
	uint8_t player2change = World.players[1].connection_status_change;
	if (GetMsDiff(ConnectionStatusTime, Get_Epoch()) > 1000 && (player1change || player2change || CurrentScreenChange)) {// might need to set loop time at beginning and call that instead
		World.players[0].connection_status_change = FALSE;
		World.players[1].connection_status_change = FALSE;
		CurrentScreenChange = FALSE;
		ST7735_FillScreen(0);                 // set screen to black
		ST7735_SetCursor(0, 0);
		switch(CurrentScreen){
			case GAME:
				ST7735_OutString("Game playing.\n");
				break;
			case PAUSE:
				ST7735_OutString("Game paused.\n");
				break;
		}
		ST7735_SetCursor(0, 1);
		ConnectionStatusTime = Get_Epoch();
		for(index = 0; index < NUM_PLAYERS; index++){
			if(Get_Epoch() - Get_Last_Update_Time(index) > 2000){
				World.players[index].connection_status = FALSE;
			}
			else{
				World.players[index].connection_status = TRUE;
			}
			ST7735_OutString("Player ");
			ST7735_OutUDec(index + 1);
			ST7735_OutString(":\n");
			if(World.players[index].connection_status == TRUE){
				ST7735_OutString("Connected\n");
			}
			else{
				ST7735_OutString("Disconnected\n");
			}
		}
	}
}
/***************** DisplayEndScreen ****************
	Displays the ends screen to the LCD
	Inputs: none
	Outputs: none
*/
void DisplayEndScreen(void){
	ST7735_FillScreen(0);                 // set screen to black
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Player ");
	ST7735_OutUDec(WinningPlayer + 1);
	ST7735_OutString(" Wins!\nPress any console\nbutton to play\nagain.");
}

/***************** Init ****************
	Initializes the clock, lcd, buttons, time, random number generator,
	esp and UART
	Inputs: none
	Outputs: none
*/
void Init(void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);
	LED_Init();
  Output_Init();       // UART0 only used for debugging
	LCD_Init();
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Game playing.");
	Buttons_Init();
	Time_Init();
	Random_Init(0);				// using same seed
	printf("\n\r-----------\n\rSystem starting...\n\r");
  ESP8266_Init(115200, SOFTAP);      // connect to access point, set up as client
	ESP8266_GetVersionNumber();
	ESP8266_GetIPAddress();
	LED_GreenOn();
	
	GameInit();
}
/***************** GameInit ****************
	Initializes all variables needed to start the game
	Inputs: none
	Outputs: none
*/
void GameInit(void) {
	CurrentScreen = GAME;							// set to game screen for testing
	Set_Current_Screen(GAME);
	Make_Map(&World);
	LastUpdateTimeMs = 0;
	ConnectionStatusTime = Get_Epoch();
	CurrentGameTime = Get_Epoch();
	World.numActivePowerups = 0;
	World.active_pointid = -1;
	CurrentScreenChange = FALSE;
	WinningPlayer = -1;
	uint8_t index;
	for(index = 0; index < NUM_MAP_POWERUPS; index++){
		World.active_powerups[index].lifetime_start = CurrentGameTime;
	}
	Packet_Read_Init();
}


