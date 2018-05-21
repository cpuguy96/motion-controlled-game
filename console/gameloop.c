
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gameloop.h"


#include "../../inc/tm4c123gh6pm.h"
#include "UART.h"						// PA1-0 for UART; UART0
#include "esp8266.h"				// PB0,1,5 (TX,RX,CH_SEL); UART1
#include "LED.h"						// PF4-0 for led and switches (keep)
#include "ST7735.h"					// PA2,3,5,6,7 lcd; PD7 sd card
#include "buttons.h"				// Uses PE2-0, timer 3
#include "gamemodel.h"
#include "time.h"						// timer 2
#include "Packet.h"
#include "lcd.h"
#include "trig.h"
#include "random.h"
#include "fixed.h" 

// prototypes for functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

//game functions
void CheckMovement(uint8_t playerid, updatepacket packet, uint32_t updateTimeDiff);
uint8_t CheckValidPos(pixel_pos loc);
pixel_pos GetNewLocationPlayer(uint8_t playerid, uint32_t updateTimeDiff, int8_t x_js, int8_t y_js);
pixel_pos GetNewLocation(pixel_pos loc, int32_t xspeed, int32_t yspeed, uint32_t updateTimeDiff);
void DelayMs(uint32_t n);

void CheckAttackMoves(uint8_t playerid, updatepacket packet);
void UpdateDamageCollision(void);
void UpdateActivePowerups(uint32_t updateTimeDiff);
uint8_t SpawnWeaponDamage(uint8_t playerid, powerup effect, int8_t weapontype);
uint8_t CheckCollision(player p, powerup effect);
void CheckPowerupCollision(void);
void UpdatePlayerStats(uint8_t playerid, uint32_t updateTimeDiff); 


//PUBLIC FUNCTIONS

/***************** SendGameData ****************
	Sends game status to UART - Update View with most recent model information
	Inputs: debugMode - 1 for debug mode with brackets, 0 for none debug mode
	Outputs: none
*/
void SendGameData(uint8_t debugMode){
	uint8_t index;
	if(debugMode){
		printf("+GAME{");
			printf("%d,", CurrentScreen);
			printf("{"); // map data
				printf("%d,%d,%d,", CurrentGameTime, World.score_limit, NUM_PLAYERS);
				printf("{");		// players
					for(index = 0; index < NUM_PLAYERS; index ++){
						player p = World.players[index];
						uint8_t index2;
						printf("{");		// player
							printf("%d,%d,%d,%d,%d,%d,", p.playerid, p.lives, p.score, p.current_direction, p.current_weapon_m + 1, p.current_weapon_r + 1);	//adding 1 to curr weapon so that 0 means nothing current
								printf("{");	// player location
									pixel_pos loc = p.location;
									printf("%d,%d,", loc.x, loc.y);
								printf("}");	// end player location
							printf("[");
								for(index2 = 0; index2 < NUM_EFFECTS; index2++){
									printf("%d,", p.stats[index2]);
								}
							printf("]");
							printf("[");
								for(index2 = 0; index2 < NUM_EFFECTS; index2++){
									printf("%d,", p.stats_timers[index2]);
									if(index2 != NUM_EFFECTS - 1){
									}
								}
							printf("]");
							printf("[");
								for(index2 = 0; index2 < NUM_WEAPONS; index2++){
									printf("%d,", p.weapons[index2].active);
								}
							printf("]");
							printf("%d,", p.weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.active);	
							printf("%d,", p.isMoving);								
							printf("}");
						
						// end player
					}
				printf("}");	// end players
				int8_t currentCP = World.active_pointid;
				printf("{");	// capturepoints
					if(currentCP == -1){	// need to check in display if no capture point is active
						printf("%d,%d,%d,", -1, -1, 0);
						printf("{"); 	// cp location
							printf("%d,%d", 0, 0);
						printf("}");	// end cp location
					}
					else{
						capturepoint cp = World.cp[currentCP];
						printf("%d,%d,%d,", cp.pointid, cp.current_playerid, cp.percent_cap);
						printf("{"); 	// cp location
							block_pos loc = cp.location;
							printf("%d,%d", loc.x, loc.y);
						printf("}");	// end cp location
					}
				printf("}");	// end capture points
			printf("},");	// end map
			printf("%d,", World.numActivePowerups);
			uint8_t powerupIndex = 0;
			for(index = 0; (index < MAX_NUM_POWERUPS) && (powerupIndex < World.numActivePowerups); index++){
				powerup p = World.active_powerups[index];
				if(p.active){
					powerupIndex ++;
					printf("%d,%d,%d,%d,%d,%d,%d,", p.weapon_type, p.hitbox_x, p.hitbox_y, p.location.x, p.location.y, p.direction, p.type);
				}		
			}
		printf("}GAME-\n\r");	// end game
	}
	else{
	printf("+GAME{");
		printf("%d,", CurrentScreen);
			printf("%d,%d,%d,", CurrentGameTime, World.score_limit, NUM_PLAYERS);
				for(index = 0; index < NUM_PLAYERS; index ++){
					player p = World.players[index];
					uint8_t index2;
						printf("%d,%d,%d,%d,%d,%d,", p.playerid, p.lives, p.score, p.current_direction, p.current_weapon_m + 1, p.current_weapon_r + 1);	//adding 1 to curr weapon so that 0 means nothing current
								pixel_pos loc = p.location;
								printf("%d,%d,", loc.x, loc.y);
							for(index2 = 0; index2 < NUM_EFFECTS; index2++){
								printf("%d,", p.stats[index2]);
							}
							for(index2 = 0; index2 < NUM_EFFECTS; index2++){
								printf("%d,", p.stats_timers[index2]);
							}
							for(index2 = 0; index2 < NUM_WEAPONS; index2++){
								printf("%d,", p.weapons[index2].active);
							}
						printf("%d,", p.weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.active);	
						printf("%d,", p.isMoving);
			// end player
				}
			int8_t currentCP = World.active_pointid;
				if(currentCP == -1){	// need to check in display if no capture point is active
					printf("%d,%d,%d,", -1, -1, 0);
						printf("%d,%d,", 0, 0);
				}
				else{
					capturepoint cp = World.cp[currentCP];
					printf("%d,%d,%d,", cp.pointid, cp.current_playerid, cp.percent_cap);
						block_pos loc = cp.location;
						printf("%d,%d,", loc.x, loc.y);
				}
		printf("%d,", World.numActivePowerups);
		uint8_t powerupIndex = 0;
		for(index = 0; (index < MAX_NUM_POWERUPS) && (powerupIndex < World.numActivePowerups); index++){
			powerup p = World.active_powerups[index];
			if(p.active){
				powerupIndex ++;
				printf("%d,%d,%d,%d,%d,%d,%d,", p.weapon_type, p.hitbox_x, p.hitbox_y, p.location.x, p.location.y, p.direction, p.type);
			}		
		}
	printf("}GAME-\n\r");	// end game
	}
}
/***************** UpdateModel ****************
	Updates player and game information - Update Model with most recent controller information
	Inputs: none
	Outputs: none
*/
void UpdateModel(void) {
	uint32_t updateTimeDiff = GetMsDiff(LastUpdateTimeMs, CurrentGameTime);
	LastUpdateTimeMs = CurrentGameTime;

	UpdateActivePowerups(updateTimeDiff);		//so moving range powerups start by the people and not one step away
	for (uint8_t p = 0; p < NUM_PLAYERS; p++) {
		player playerP = World.players[p];
		updatepacket packet = Get_Player_Data(playerP.playerid);
		
		//update movement, update game model, collisions, etc for one player
		UpdatePlayerStats(playerP.playerid, updateTimeDiff);
		CheckMovement(playerP.playerid, packet, updateTimeDiff);
		CheckAttackMoves(playerP.playerid, packet);					// sets current weapon active if swipe gesture/button pressed; need to put cooldown on activation			
	}
	//stuff for all players
	CheckPowerupCollision();													// checks if any active damage collides with player; updates player stats if true

}

/***************** GetMsDiff ****************
	Caluclates millisecond time difference between two
	times
	Inputs: old - previous time
					newTime - newer time
	Outputs: millisecond difference between the two times
*/
uint32_t GetMsDiff(uint32_t old, uint32_t newTime) {
	if (newTime < old) {
		return newTime + 1000 - old;
	}
	else {
		return newTime - old;
	}
}



//PRIVATE FUNCTIONS

/***************** UpdatePlayerStats ****************
	Updates player status 
	Inputs: playerid - player id
					updateTimeDiff - time since last game update
	Outputs: none
*/
void UpdatePlayerStats(uint8_t playerid, uint32_t updateTimeDiff) {
	player p = World.players[playerid];
	for (uint8_t i = 0; i < NUM_EFFECTS; i++) {
		if (p.stats_timers[i] > 0) {
			//effect timers
			World.players[playerid].stats_timers[i]-= updateTimeDiff;
			switch(i){
				case HEALTH:
					if(p.stats[HEALTH] + HEALTH_BOOST <= MAX_HEALTH){
						World.players[playerid].stats[HEALTH] += HEALTH_BOOST;
					}
					else{
						World.players[playerid].stats[HEALTH] = MAX_HEALTH;
					}
					break;
				case DMG:
					if(p.stats[HEALTH] - p.stats[DMG] >= 0){
						World.players[playerid].stats[HEALTH] -= World.players[playerid].stats[DMG];
					}
					else{
						World.players[playerid].stats[HEALTH] = 0;
					}
					break;
				case SPEED:
					World.players[playerid].stats[SPEED] = SPEED_BOOST;
					break;
				case AMMO:
					if(p.weapons[p.current_weapon_r].ammo + AMMO_BOOST <= MAX_AMMO){
						World.players[playerid].weapons[NUMWEAPONTYPES_M + p.current_weapon_r].ammo += AMMO_BOOST;
					}
					World.players[playerid].weapons[NUMWEAPONTYPES_M + p.current_weapon_r].ammo = MAX_AMMO;
					break;
				case STRENGTH:
					World.players[playerid].stats[STRENGTH] = STRENGTH_BOOST;
					break;
			}
		}
		else {
			switch(i){
				case AMMO:
					World.players[playerid].stats[AMMO] = p.weapons[p.current_weapon_r + NUMWEAPONTYPES_M].ammo;
					break;
				case DMG:
					World.players[playerid].stats[DMG] = 0;
				break;
				case SPEED:
					World.players[playerid].stats[SPEED] = START_SPEED;
					break;
				case STRENGTH:
					World.players[playerid].stats[STRENGTH] = START_STRENGTH;
					break;
			}
		}
	}
}
/***************** CheckGestureBounds ****************
	Checks if gesture magnitues are within bounds
	Inputs: x - gesture magnitude
	Outputs: 1 if magnitude is valid, 0 if magnitude is invalid
*/
uint8_t CheckGestureBounds(int8_t x) {
	return x != 0 && (x >= -3 && x <= 3);
}
/***************** CheckAttackMoves ****************
	Checks if player has issued an attack
	Inputs: playerid - player id
					packet - packet of latest player information 
	Outputs: none
*/
void CheckAttackMoves(uint8_t playerid, updatepacket packet){
	player p = World.players[playerid];
	if (packet.b_js_t && p.switchWeaponCooldown < CurrentGameTime) {
		uint8_t numRangeAttacks = 3;
		World.players[playerid].current_weapon_r = (World.players[playerid].current_weapon_r + 1) % numRangeAttacks;
		World.players[playerid].switchWeaponCooldown = CurrentGameTime + 500; 
	}
	
	if(CheckGestureBounds(packet.x_gesture) || CheckGestureBounds(packet.y_gesture) || CheckGestureBounds(packet.z_gesture)){ // swipe movement detected in any direction; and main button(can remove later)
		if(CurrentGameTime >= p.weapons[p.current_weapon_m].effect.cooldown + p.weapons[p.current_weapon_m].effect.lifetime_start){ // checks if current time is greater than cooldown time
			World.players[playerid].weapons[p.current_weapon_m].effect.active = SpawnWeaponDamage(playerid, p.weapons[p.current_weapon_m].effect, MELEE);
		}
	}
	else {
		World.players[playerid].weapons[p.current_weapon_m].effect.active = FALSE;
	}
	if (packet.b_primary_t == 1) {
		if(CurrentGameTime >= p.weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.cooldown + p.weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.lifetime_start){ // checks if current time is greater than cooldown time
			if (World.players[playerid].stats[AMMO] > 0) {
				World.players[playerid].weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.active = SpawnWeaponDamage(playerid, p.weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect, RANGE);
				World.players[playerid].weapons[NUMWEAPONTYPES_M + p.current_weapon_r].ammo -= 1;
				World.players[playerid].stats[AMMO] -= 1;
			}
		}
	}
	else{
		World.players[playerid].weapons[NUMWEAPONTYPES_M + p.current_weapon_r].effect.active = FALSE;
	}
}
/***************** SpawnWeaponDamage ****************
	Places a damage powerup on the screen
	Inputs: playerid - player id
					effect - powerup containing damage stats 
					weapontype - type of weapon damage that is spawned
	Outputs: returns 1 if powerup is able to be placed on the screen
*/
uint8_t SpawnWeaponDamage(uint8_t playerid, powerup effect, int8_t weapontype){
	// returns true if weapon was able to be placed onto world
	player p = World.players[playerid];

	int16_t rotDeg = p.current_direction;
	int16_t pixel_x = p.location.x;
	int16_t pixel_y = p.location.y;
	rotDeg = rotDeg % 360;						// normalizes just in case
	if(rotDeg < 0){
		rotDeg *= -1;
	}
	effect.active = TRUE;  
	effect.direction = rotDeg;
	
	if((rotDeg >= 0 && rotDeg < 45) || (rotDeg >= 315 && rotDeg < 360)){ // looking east
		effect.location.x = pixel_x - effect.hitbox_x;
		effect.location.y = pixel_y + PLAYER_SIZE / 2 - effect.hitbox_y / 2;
	}
	else if(rotDeg >= 45 && rotDeg < 135){ // looking north
		effect.location.x = pixel_x + PLAYER_SIZE / 2 - effect.hitbox_x / 2;
		effect.location.y = pixel_y - effect.hitbox_y;
	}
	else if(rotDeg >= 135 && rotDeg < 225){ // looking west
		effect.location.x = pixel_x + PLAYER_SIZE;
		effect.location.y = pixel_y + PLAYER_SIZE / 2 - effect.hitbox_y / 2;
	}
	else if(rotDeg >= 225 && rotDeg < 315){ // looking south
		effect.location.x = pixel_x + PLAYER_SIZE / 2 - effect.hitbox_x / 2;
		effect.location.y = pixel_y + PLAYER_SIZE;
	}
	else{ // should be invalid rotation; east by default
		effect.location.x = pixel_x - effect.hitbox_x;
		effect.location.y = pixel_y + PLAYER_SIZE / 2 - effect.hitbox_y / 2;
	}
	
	//is updating World.players[playerid].weapons[currWeapon].effect.lifetime_start = effect.lifetime_start; necessary?
	uint8_t currWeapon = 0;
	if (weapontype == MELEE) {
		currWeapon = p.current_weapon_m;
	}
	if (weapontype == RANGE) {
		currWeapon = p.current_weapon_r + NUMWEAPONTYPES_M;
	}
	
	if(CurrentGameTime >= effect.cooldown + effect.lifetime_start){ // checks if current time is greater than cooldown time
		for(uint8_t index = NUM_MAP_POWERUPS; index < MAX_NUM_POWERUPS; index++){ // finds next avaliable active powerup slot
			powerup active = World.active_powerups[index];
			if(active.active == FALSE){
				effect.lifetime_start = CurrentGameTime;
				if(effect.type == DMG){
					World.players[playerid].weapons[currWeapon].effect.lifetime_start = effect.lifetime_start;
					effect.boost_amount *= World.players[playerid].stats[STRENGTH];
				}
				World.active_powerups[index] = effect;
				World.numActivePowerups ++;// increm active num
				return TRUE;
			}
		}
	}	
	return FALSE;
}

/***************** UpdateActivePowerups ****************
	Updates active powerups in the game 
	Inputs: updateTimeDiff - time since last game update
	Outputs: none
*/
void UpdateActivePowerups(uint32_t updateTimeDiff){
	uint8_t index, powerupIndex = 0;
	for(index = NUM_MAP_POWERUPS; (index < MAX_NUM_POWERUPS) && (powerupIndex < World.numActivePowerups); index++){
		powerup active = World.active_powerups[index];
		if(active.active == TRUE) {
			if (CurrentGameTime >= active.lifetime_start + active.lifetime) {
				powerupIndex++;
				World.active_powerups[index].active = FALSE;
				World.numActivePowerups --;
			}
			else if (active.location.x < 0 || active.location.y < 0 || active.location.x >= MAX_X || active.location.y >= MAX_Y ) {
				World.active_powerups[index].active = FALSE;
				World.numActivePowerups --;
			}
			else if (active.speed > 0) {
				int32_t xspeed = -1 * active.speed * Cosine(active.direction) * 2; // times 2 to account that player can move at 2x speed
				int32_t yspeed = -1 * active.speed * Sine(active.direction) * 2;
				pixel_pos loc = GetNewLocation(active.location, xspeed, yspeed, updateTimeDiff);
				if(loc.x < 0 || loc.y < 0 || loc.x >= MAX_X || loc.y >= MAX_Y){
					World.active_powerups[index].active = FALSE;
					World.numActivePowerups --;
				}
				else{
					World.active_powerups[index].location = loc;
				}
			}

		}
	}
	// placing powerups that can be picked up
	for(index = 0; index < NUM_MAP_POWERUPS; index++){
		powerup mapPow = World.active_powerups[index];
		if(mapPow.active == FALSE && CurrentGameTime >= mapPow.lifetime_start + mapPow.cooldown){
			pixel_pos powLoc = mapPow.location;
			uint8_t randomType = Random()%NUM_EFFECTS;
			while(randomType == DMG){
				randomType = Random()%NUM_EFFECTS;
			}
			switch(randomType){
				case HEALTH:
					World.active_powerups[index] = Make_Powerup(HEALTH, -1, HEALTH_BOOST, 1, PIXELS_PER_BLOCK, PIXELS_PER_BLOCK, powLoc, MAX_32_INT, 0, MAP_POWERUP_COOLDOWN);
					break;
				case SPEED:
					World.active_powerups[index] = Make_Powerup(SPEED, -1, SPEED_BOOST, 1, PIXELS_PER_BLOCK, PIXELS_PER_BLOCK, powLoc, MAX_32_INT, 0, MAP_POWERUP_COOLDOWN);
					break;
				case AMMO:
					World.active_powerups[index] = Make_Powerup(AMMO, -1, AMMO_BOOST, 1, PIXELS_PER_BLOCK, PIXELS_PER_BLOCK, powLoc, MAX_32_INT, 0, MAP_POWERUP_COOLDOWN);
					break;
				case STRENGTH:
					World.active_powerups[index] = Make_Powerup(STRENGTH, -1, STRENGTH_BOOST, 1, PIXELS_PER_BLOCK, PIXELS_PER_BLOCK, powLoc, MAX_32_INT, 0, MAP_POWERUP_COOLDOWN);
					break;
			}
			World.active_powerups[index].active = TRUE;
			World.numActivePowerups ++;
		}
	}
}
/***************** CheckPowerupCollision ****************
	Checks if a player has collided with a powerup and sets 
	status timers if so
	Inputs: none
	Outputs: none
*/
void CheckPowerupCollision(void) {	// change so player can't collide with their own damage
	for(uint8_t pIndex = 0; pIndex < NUM_PLAYERS; pIndex++){
		uint8_t index2;
		for(index2 = 0; index2 < MAX_NUM_POWERUPS; index2++){
			powerup effect = World.active_powerups[index2];
			if (effect.active == TRUE) {
				if(CheckCollision(World.players[pIndex], effect)) {
					World.active_powerups[index2].active = FALSE;
					World.active_powerups[index2].lifetime_start = CurrentGameTime;
					World.numActivePowerups --;
					switch(effect.type){
						case HEALTH:
							World.players[pIndex].stats_timers[HEALTH] = effect.boost_time;
							break;
						case DMG:
							if((World.players[pIndex].stats[DMG] == 0)&& World.players[pIndex].stats[DMG] + effect.boost_amount <= 100){ // setting max damange amount
								// only one source of damage can effect player
								World.players[pIndex].stats[DMG] = effect.boost_amount;
							}
							else{
								World.players[pIndex].stats[DMG] = 100;
							}
							if((World.players[pIndex].stats_timers[DMG] == 0) && (World.players[pIndex].stats_timers[DMG] + effect.boost_time <= 100)){ // setting max damange amount
								// only recent attack effects player; kinda like iframes
								World.players[pIndex].stats_timers[DMG] = effect.boost_time;
							}
							else{
								World.players[pIndex].stats_timers[DMG] = 100;
							}
							break;
						case SPEED:
							World.players[pIndex].stats_timers[SPEED] = effect.boost_time;							
							break;
						case AMMO:
							World.players[pIndex].stats_timers[AMMO] = effect.boost_time;														
							break;
						case STRENGTH:
							World.players[pIndex].stats_timers[STRENGTH] = effect.boost_time;														
							break;
					}
				}
			}
		}
	}
}
/***************** CheckCollision ****************
	Checks if player has collided with a powerup
	Inputs: p - player 
					effect - powerup to check player collision with
	Outputs: 1 if player collided with powerup, 0 otherwise
*/
uint8_t CheckCollision(player p, powerup effect){ // change to check all 4 corners of player
	if( !( 	(p.location.x  > effect.location.x + effect.hitbox_x - 1) ||
					(p.location.x + PLAYER_SIZE - 1 < effect.location.x) 			||
					(p.location.y  > effect.location.y + effect.hitbox_y - 1) ||
					(p.location.y + PLAYER_SIZE - 1 < effect.location.y)  		) ){
		return 1;
	}
	return 0;
}
/***************** CheckMovement ****************
	Checks if player can move to new location and updates 
	player location accordingly
	Inputs: playerid - player id
					packet - packet containing player information
					updateTimeDiff - time since last game update
	Outputs: none
*/
void CheckMovement(uint8_t playerid, updatepacket packet, uint32_t updateTimeDiff) {
	packet.y_rot = packet.y_rot % 360;						// normalizes just in case
	if(packet.y_rot < 0){
		packet.y_rot *= -1;
	}
	if (packet.y_rot <= 180) {
		World.players[playerid].current_direction = (ROT_MULT * packet.y_rot + 90) % 360;
	}
	else {
		World.players[playerid].current_direction = (360 - ROT_MULT * (360 - packet.y_rot) + 90) % 360;
	}
	
	
	pixel_pos loc = GetNewLocationPlayer(playerid, updateTimeDiff, packet.x_js, packet.y_js);
	if (CheckValidPos(loc)) {
		//printf("+DEBUG valid pos\n\r");
		World.players[playerid].location = loc;
	}
	
	if (packet.x_js | packet.y_js) {
		World.players[playerid].isMoving = 1;
	}
	else {
		World.players[playerid].isMoving = 0;
	}

}

//update Player Location with packet.x_js, packet.y_js
/***************** GetNewLocationPlayer ****************
	Calculates the new location of the player based on
	joystick movement
	Inputs: playerid - player id
					updateTimeDiff - time since last game update
					x_js - x magnitude of the player's joystick
					y_js - y magnitude of the player's joystick
	Outputs: new pixel location 
*/
pixel_pos GetNewLocationPlayer(uint8_t playerid, uint32_t updateTimeDiff, int8_t x_js, int8_t y_js) {
	pixel_pos loc = World.players[playerid].location;
	int16_t speed = World.players[playerid].stats[SPEED] - 32 * Check_Slow(World.players[playerid].location.x, World.players[playerid].location.y);
	pixel_pos loc_new = GetNewLocation(loc, -1 * x_js * speed * 1000, y_js * speed * 1000, updateTimeDiff);
	return loc_new;
}

//speeds are *1000 for precision with angled movement
/***************** GetNewLocationPlayer ****************
	Calculates a new location based on speed
	Inputs: loc - pixel location
					xspeed - x speed 
					yspeed - y speed
					updateTimeDiff - time since last game update
	Outputs: new pixel location 
*/
pixel_pos GetNewLocation(pixel_pos loc, int32_t xspeed, int32_t yspeed, uint32_t updateTimeDiff) {
	pixel_pos loc_new = loc;
	loc_new.x += (xspeed * (int32_t)updateTimeDiff) / (1000000);
	loc_new.y += (yspeed * (int32_t)updateTimeDiff) / (1000000);
	return loc_new;
}

/***************** CheckValidPos ****************
	Checks if pixel location is valid to move to
	Inputs: loc - pixel location
	Outputs: 1 if position is valid, 0 otherwise
*/
uint8_t CheckValidPos(pixel_pos loc) {
	//check for movement bound by the edge of the map
	if ( (loc.x < 0) || (loc.y < 0) || ((loc.x >= MAX_X - PIXELS_PER_BLOCK) || (loc.y >= MAX_Y - PIXELS_PER_BLOCK) ))
	{
		return FALSE;
	}
	//check for block types where movement is not allowed, no movement if so
	if ( (Check_Blocked(loc.x, loc.y)) )
	{
		return FALSE;
	}

	return TRUE;
}

