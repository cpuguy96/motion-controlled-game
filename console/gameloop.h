#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "gamemodel.h"
#define ROT_MULT 1		//multiplier for the look rotation


//functions to update the model and view
void UpdateModel(void);									//Update Model with most recent controller information
void SendGameData(uint8_t debugMode);		//Update View with most recent model information

//helpful function to prevent long delays from causing problems with the loop
uint32_t GetMsDiff(uint32_t old, uint32_t newTime);

//current status of game
static uint8_t CurrentScreen;					// start, pause, selection, game screens
static uint32_t CurrentGameTime;
static uint32_t LastUpdateTimeMs;

//game model
static map World;

#endif

