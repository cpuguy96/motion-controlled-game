#include <stdint.h>
#include <string.h>
#include "gamemodel.h"
#include "time.h"
#include "private_gamemodel.h"

/***************** Make_Weapon ****************
	Creates a weapon struct with specified properties
	Inputs: weaponid - id assoicated with weapon
					type - type of weapon
					loc - pixel location 
	Outputs: none
*/
weapon Make_Weapon(uint8_t weaponid, uint8_t type, pixel_pos loc){
	weapon w;
	powerup e;
	w.weaponid = weaponid;
	if(IS_MELEE(type)){
		switch(type){
		case SWORD:
			e = Make_Powerup(DMG, SWORD, 20, 1, PLAYER_SIZE / 2, PLAYER_SIZE / 2, loc, 1, 0, 500);
			w.ammo = -1;					// -1 ammo means infinite 
			break;
		case BAT:
			e = Make_Powerup(DMG, BAT, 20, 1, PLAYER_SIZE / 2, PLAYER_SIZE / 2, loc, 1, 0, 500);
			w.ammo = -1;					// -1 ammo means infinite 
			break;
		case SOLDERINGIRON:
			e = Make_Powerup(DMG, SOLDERINGIRON, 20, 1, PLAYER_SIZE / 2, PLAYER_SIZE / 2, loc, 1, 0, 500);

			w.ammo = -1;					// -1 ammo means infinite 
			break;
		}
	}
	else{
		uint8_t rangetype = type - NUMWEAPONTYPES_M;
		switch(rangetype){
		case HANDGUN:
			e = Make_Powerup(DMG, HANDGUN + NUMWEAPONTYPES_M, 15, 1, 4, 4, loc, 5000, 16 * PIXELS_PER_BLOCK, 1000);
			w.ammo = 100;
			break;
		case RIFLE:
			e = Make_Powerup(DMG, RIFLE + NUMWEAPONTYPES_M, 2, 1, 2, 2, loc, 8000, 12 * PIXELS_PER_BLOCK, 200);		
			w.ammo = 200;
			break;
		case SHOTGUN:
			e = Make_Powerup(DMG, SHOTGUN + NUMWEAPONTYPES_M, 10, 1, 10, 10, loc, 2000, 8 * PIXELS_PER_BLOCK, 500);		
			w.ammo = 50;
			break;
		}
	}
	w.effect = e;
	return w;
}
/***************** Make_Powerup ****************
	Creates a powerup struct with specified properties
	Inputs: type - type of powerup
					weapon_type - weapon type
					boost_ammount -	ammount of boost effect
					boost_time - duration of boost effect
					hitbox_x - x length of hitbox
					hitbox_y - y length of hitbox
					loc - pixel location
					lifetime - duration on the screen
					speed -	how fast powerup moves on screen
					cooldown - time before powerup can be created again
	Outputs: powerup struct with specified properties
*/
powerup Make_Powerup(int8_t type, int8_t weapon_type, int16_t boost_amount, int16_t boost_time, int16_t hitbox_x, int16_t hitbox_y, pixel_pos loc, uint32_t lifetime, int16_t speed, uint32_t cooldown){
	powerup p;
	p.type = type;
	switch(type){
		case HEALTH:
			p.boost_amount = HEALTH_BOOST;
			p.boost_time = 1; 		// only 1 "tick" of health increase
			p.hitbox_x = PIXELS_PER_BLOCK;				// hitbox of 0 means actual powerup instead of weapon
			p.hitbox_y = PIXELS_PER_BLOCK;
			p.lifetime = MAX_32_INT;			// alive until collsion; makes it live till the max of 32bit number
			p.speed = 0;
			p.weapon_type = -1; 	// -1 since not a weapon
			break;
		case DMG:								// damage done by weapon
			p.boost_amount = boost_amount;
			p.boost_time = 1; 				// having 1 boostime (for D.O.T) means damage lasts 1 game loop after in contact
			p.hitbox_x = hitbox_x;
			p.hitbox_y = hitbox_y;
			p.lifetime = lifetime;				// how long damage effect lasts on the map; melee is instant so probably 1 loop
			p.speed = speed;									// 0 for now since we only have melee; 0 speed means stationary
			p.weapon_type = weapon_type; 	
			break;
		case SPEED:
			p.boost_amount = SPEED_BOOST;		// 100% speed addition (assume normal speed is 1)
			p.boost_time = 10000; 		// boost time of 10 seconds
			p.hitbox_x = PIXELS_PER_BLOCK;				// hitbox of 0 means actual powerup instead of weapon
			p.hitbox_y = PIXELS_PER_BLOCK;
			p.lifetime = MAX_32_INT;
			p.speed = 0;
			p.weapon_type = -1; 	// -1 since not a weapon
			break;
		case AMMO:
			p.boost_amount = AMMO_BOOST;	// fixed 20 additional ammo; can be changed 
			p.boost_time = 1; 		// only 1 "tick" of ammo increase
			p.hitbox_x = PIXELS_PER_BLOCK;				// hitbox of 0 means actual powerup instead of weapon
			p.hitbox_y = PIXELS_PER_BLOCK;
			p.lifetime = MAX_32_INT;
			p.speed = 0;
			p.weapon_type = -1; 	// -1 since not a weapon
			break;
		case STRENGTH:
			p.boost_amount = STRENGTH_BOOST;		// 100% strength addition (assume normal strength is 1)
			p.boost_time = 10000; 		// only 1 "tick" of strength increase
			p.hitbox_x = PIXELS_PER_BLOCK;				// hitbox of 0 means actual powerup instead of weapon
			p.hitbox_y = PIXELS_PER_BLOCK;
			p.lifetime = MAX_32_INT;
			p.speed = 0;
			p.weapon_type = -1; 	// -1 since not a weapon	
			break;
	}
	p.cooldown = cooldown;
	p.active = 0;
	p.location = loc;
	p.direction = 0;
	return p;
}
/***************** Make_CapturePoint ****************
	Creates a capturepoint struct with specified properties
	Inputs: pointid - id number 
					location - block location
	Outputs: capturepoint with specified properties
*/
capturepoint Make_CapturePoint(uint8_t pointid, block_pos location){
	capturepoint cp;
	cp.pointid = pointid;
	cp.current_playerid = -1;
	cp.percent_cap = 0;
	cp.location = location;
	return cp;
}
/***************** Make_Player ****************
	Creates a player struct with specified properties
	Inputs: playerid- id number
					location - pixel location
	Outputs: player with specified properties
*/
player Make_Player(uint8_t playerid, pixel_pos location){
	player p;
	uint8_t index;
	p.playerid = playerid;
	p.lives = MAX_LIVES;
	p.score = 0;
	p.current_direction = EAST;
	p.direction_flipped = 0;
	//enum playerstats {HEALTH, DMG, SPEED, AMMO, STRENGTH, NUM_EFFECTS};
	int16_t initialStats[NUM_EFFECTS] = {START_HEALTH, 0, START_SPEED, START_AMMO, START_STRENGTH};
	for(index = 0; index < NUM_EFFECTS; index++){
		p.stats[index] =  initialStats[index];
		p.stats_start[index] = initialStats[index];
	}
	memset(p.stats_timers, 0, NUM_EFFECTS * sizeof(int16_t));
	int16_t initialWeapons[NUM_WEAPONS] = {1, 0, 0, 0, 0, 0}; // 0 - 2 are melee, 3 - 5 are ranged
	for(index = 0; index < NUM_WEAPONS; index++){
		p.weapons[index] = Make_Weapon(playerid, index, location);
		p.weapons[index].active = 0;
		if(initialWeapons[index] == 1){				// set the first weapon as active
			p.weapons[index].active = 1;
		}
	}
	p.current_weapon_m = 0;																		// initial weapon is a sword
	p.current_weapon_r = 0;	
	p.location = location;
	p.isMoving = 0;
	p.switchWeaponCooldown = 0;
	p.connection_status = 0;
	p.connection_status_change = 0;
	return p;
}

/***************** Make_Map ****************
	Creates a map struct with specified properties
	Inputs: m - pointer to map struct 
	Outputs: none 
*/
void Make_Map(map* m){
	uint8_t index;
	
	m->time = TIME_LIMIT;
	m->score_limit = MAX_SCORE;
	
	for(index = 0; index < NUM_PLAYERS; index++){
		pixel_pos loc = MakePixelPos(PLAYER_START_POS[index].x * PIXELS_PER_BLOCK, PLAYER_START_POS[index].y * PIXELS_PER_BLOCK);					// need to set default player positions
		player play = Make_Player(index, loc);
		m->players[index] = play;
	}
	for(index = 0; index < NUM_CAPTUREPOINTS; index++){
		block_pos loc = MakeBlockPos(3, 3);			// need to change to pick capture point position
		capturepoint cp = Make_CapturePoint(index, loc);
		m->cp[index] = cp;
	}
	// powerups not initialized since none are active at beginning
	for(index = 0; index < NUM_MAP_POWERUPS; index++){
		pixel_pos loc = {MAP_POWERUP_POS[index].x * PIXELS_PER_BLOCK, MAP_POWERUP_POS[index].y * PIXELS_PER_BLOCK};
		powerup pow = Make_Powerup(HEALTH, -1, HEALTH_BOOST, 1, PIXELS_PER_BLOCK, PIXELS_PER_BLOCK, loc, MAX_32_INT, 0, MAP_POWERUP_COOLDOWN);
		m->active_powerups[index] = pow;
	}
	for(index = NUM_MAP_POWERUPS; index < MAX_NUM_POWERUPS; index++){
		pixel_pos loc = {0, 0};
		powerup pow = Make_Powerup(DMG, -1, 0, 0, 0, 0, loc, 0, 0, 0);
		m->active_powerups[index] = pow;
	}
	m->numActivePowerups = 0;
	m->active_pointid = -1;
}
/***************** Check_Slow ****************
	Checks if object is moving to a slow block
	Inputs: newx - x location object is trying to move to
					newy - y location object is trying to move to
	Outputs: 1 if location is a slow block, 0 if location is not a slow block
*/
uint8_t Check_Slow(int16_t newx, int16_t newy) {
	int16_t centerX = (newx + PLAYER_SIZE / 2) / PIXELS_PER_BLOCK;
	int16_t centerY = (newy + PLAYER_SIZE / 2) / PIXELS_PER_BLOCK;
	return MAP_LAYOUT[centerY][centerX] == 'S';
}
/***************** Check_Blocked ****************
	Checks if object is moving to a blocked location
	Inputs: newx - x location object is trying to move to
					newy - y location object is trying to move to
	Outputs: 1 if location is blocked, 0 if location is not blocked
*/
uint8_t Check_Blocked(int16_t newx, int16_t newy) {
		int16_t newSpriteMapX = newx / PIXELS_PER_BLOCK;
		int16_t newSpriteMapY = newy / PIXELS_PER_BLOCK;
		int16_t newxMax = PIXELS_PER_BLOCK + newx - 1;
		int16_t newyMax = PIXELS_PER_BLOCK + newy - 1;
		int16_t maxSpriteMapX = newxMax / PIXELS_PER_BLOCK;
		int16_t maxSpriteMapY = newyMax / PIXELS_PER_BLOCK;
	
		uint8_t ret = (	(MAP_LAYOUT[newSpriteMapY][newSpriteMapX] == 'X') ||
					(MAP_LAYOUT[maxSpriteMapY][maxSpriteMapX] == 'X') ||
					(MAP_LAYOUT[newSpriteMapY][maxSpriteMapX] == 'X') ||
					(MAP_LAYOUT[maxSpriteMapY][newSpriteMapX] == 'X') );

		return ret;
}
/***************** MakePixelPos ****************
	Creates a pixel_pos struct with specified properties
	Inputs: x - x pixel location
					y - y pixel location
	Outputs: pixel_pos struct with specified properties
*/
pixel_pos MakePixelPos(int16_t x, int16_t y){
	pixel_pos loc = {x, y};
	return loc;
}
/***************** MakeBlockPos ****************
	Creates a block_pos struct with specified properties
	Inputs: x - x pixel location
					y - y pixel location
	Outputs: block_pos struct with specified properties
*/
block_pos MakeBlockPos(uint8_t x, uint8_t y){
	block_pos loc = {x, y};
	return loc;
}

/* updatepacket format
	uint8_t playerid; //between 0 and NUMPLAYERS
	int32_t q0;
	int32_t q1;
	int32_t q2;
	int32_t q3;
	uint8_t x_gesture;
	uint8_t y_gesture;
	uint8_t z_gesture;
	int16_t y_rot;
	uint8_t x_js;
	uint8_t y_js;
	uint8_t b_primary_t;
	uint8_t b_js_t;
	uint8_t b_3_t;
 {D:id:q0,q1,q2,q3:x_gesture,y_gesture,z_gesture:x_js,y_js:b_primary,b_js,b_3}
*/
/***************** Make_Packet ****************
	Creates a updatepacket struct with specified properties
	Inputs: playerid - player id
					q0 - quaternion 0 
					q1 - quaternion 1
					q2 - quaternion 2
					q3 - quaternion 3
					x_gesture - x gesture direction magnitude
					y_gesture - y gesture direction magnitude
					z_gesture - z gesture direction magnitude
					y_rot - y axis rotation
					x_js - x joystick direction
					y_js - y joystick direction
					b_primary_t - primary button input
					b_js_t - joystick button input
					b_3_t - reset button input
	Outputs: updatepacket struct with specified properties
*/
updatepacket Make_Packet(uint8_t playerid, int32_t q0, int32_t q1, int32_t q2, int32_t q3,
												 int8_t x_ges, int8_t y_ges, int8_t z_ges, int16_t y_rot, int8_t x_js, int8_t y_js,
												 uint8_t b_pri, uint8_t b_js, uint8_t b_3){// maybe change to setting struct in parameter
	updatepacket pack = {playerid,q0,q1,q2,q3,x_ges,y_ges,z_ges,y_rot,x_js,y_js,b_pri,b_js,b_3};
	return pack;
}

