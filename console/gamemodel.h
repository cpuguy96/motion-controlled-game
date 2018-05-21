#ifndef GAMEMODEL_H
#define GAMEMODEL_H

/*Model datatypes for MVC game architecture
 *this module can eventually hold functions that help create the structs
 *Constant structs (like weapons, heroes with starter weapons and special abilities) can live here
 */


#define MAP_X_SIZE 50			// max size the tm4c can take; might be able to increase it if we can reduce map struct size
#define MAP_Y_SIZE 40

#define PIXELS_PER_BLOCK 32	//pixels
#define PLAYER_SIZE PIXELS_PER_BLOCK // can make player smaller if needed

#define MAX_X (MAP_X_SIZE * PIXELS_PER_BLOCK) // this - 1 is max x pos
#define MAX_Y (MAP_Y_SIZE * PIXELS_PER_BLOCK)

#define PIXEL_TO_BLOCK(x) (x/PIXELS_PER_BLOCK)
#define IS_MELEE(x) (x < NUMWEAPONTYPES_M)
#define IS_RANGED(x) (x - NUMWEAPONTYPES_M  >= 0 && x - NUMWEAPONTYPES_M < NUMWEAPONTYPES_R)

#define NUM_PLAYERS 2
#define NUM_CAPTUREPOINTS 1
#define NUM_MAP_POWERUPS 7

#define LIFETAKEN_SCOREBOOST 5
#define CAPTUREPOINT_SCOREBOOST 20
#define MAX_SCORE 100

#define MAX_LIVES 5
#define MAX_NUM_POWERUPS 18

#define TIME_LIMIT 60 // in seconds

#define NORTH 90
#define EAST 0
#define SOUTH 180
#define WEST 270

#define MAX_32_INT 2147483647				// max for signed but half for unsigned

#define START_HEALTH 100
#define START_SPEED PIXELS_PER_BLOCK * 2 // 2 blocks per second
#define START_STRENGTH 1
#define START_AMMO 0

#define HEALTH_BOOST 25
#define STRENGTH_BOOST 2
#define SPEED_BOOST PIXELS_PER_BLOCK * 4 
#define AMMO_BOOST 20

#define MAX_AMMO 100
#define MAX_HEALTH 100

#define MAP_POWERUP_COOLDOWN 5000 //5 seconds right now; can make longer

#define PACKET_DELS ":,{}" // deliminator for packet message pieces

enum PACKET_TYPE{MESSAGE, PLAYER1, PLAYER2};
//open= walk, wall= not walk, death=instant death (walking off a cliff or something like that)
enum blocktype {OPEN, WALL, DEATH, NUMBLOCKTYPES};

enum playerstats {HEALTH, DMG, SPEED, AMMO, STRENGTH, NUM_EFFECTS};

enum weapontype {MELEE, RANGE, NUMWEAPONTYPES};

enum weaponid_melee {SWORD, BAT, SOLDERINGIRON, NUMWEAPONTYPES_M};

enum weaponid_range {HANDGUN, RIFLE, SHOTGUN, NUMWEAPONTYPES_R};

#define NUM_WEAPONS (NUMWEAPONTYPES_M + NUMWEAPONTYPES_R)
/* Represents a position within the map
 * x [0, MAP_X_SIZE - 1, y [0, MAP_Y_SIZE - 1], 
 * x_mini/y_mini [0, MAP_MINIBLOCK_SIZE - 1]
 */
typedef struct block_pos{
	uint8_t x;
	uint8_t y;
}block_pos;

typedef struct pixel_pos{ // = block_pos(x/y) * (x/y)_size + offset
	int16_t x;
	int16_t y;
}pixel_pos;


/* Anything that has an effect on a player
 */
typedef struct powerup {
	int8_t type; 	//playerstats enum
	int8_t weapon_type; 		// gets from weapon struct; -1 if not a weapon

	int16_t boost_amount;		//see player struct stats_boost, stats_current
	int16_t boost_time;			// boost time of -1 means infinite duration

	int8_t hitbox_x;				// hitbox of 0 means actual powerup instead of weapon
	int8_t hitbox_y;

	pixel_pos location;

	uint32_t lifetime;		//how often they last (mostly used for weapon damage) -1 means alive until collision
	uint32_t lifetime_start;
	uint32_t cooldown;							// delay before the powerup can be used again
	//powerups can move or be launched from a weapon
	//int16_t current_direction;		//degrees rotation; 
	int16_t speed;								// projectile speed
	int16_t direction;
	uint8_t active;
}powerup;

/* Only need one object that reperesents each weapon type
 * The player stores a weaponid and ammo amount, the struct just hold info about each weaponid
 */
typedef struct weapon {
	uint8_t weaponid;	//weaponid_melee, weaponid_range enums
	uint8_t type;		//weapontype enum; 0 - 2 are melee, 3 - 5 are ranged
	
	int16_t ammo;		// -1 ammo means infinite
	powerup effect; //weapons either launch a projectile or cause an effect (like damage), which can be handled by the powerup struct
	uint8_t active;
}weapon;

typedef struct player {
	uint8_t playerid; //between 0 and NUMPLAYERS
	
	int8_t lives;
	int16_t score;
	int16_t current_direction;		//degrees rotation
	uint8_t direction_flipped;
	pixel_pos location;

	int16_t stats_start[NUM_EFFECTS];		//initial values for the stats
	int16_t stats[NUM_EFFECTS];				//current values for the stats; HEALTH, DMG, SPEED, AMMO, STRENGTH
	int16_t stats_timers[NUM_EFFECTS];		//timer for powerups that temporarily boost

	weapon weapons[NUM_WEAPONS];		//holds an enum that represents the type of weapon
	
	uint8_t current_weapon_m;
	uint8_t current_weapon_r;
	uint8_t isMoving;
	
	int32_t switchWeaponCooldown;
	//maybe special ability, like an effect type an amout? 
	uint8_t connection_status;
	uint8_t connection_status_change;
}player;

/* The map will have objects that players capture for points
 */
typedef struct capturepoint {
	uint8_t pointid;
	int8_t current_playerid;

	int8_t percent_cap;
	block_pos location;
}capturepoint;

/* An array of blocks, list of players and objects
 */
typedef struct map {
	int16_t time;
	int16_t score_limit;
	int8_t active_pointid;		//only one active at a time, random next; -1 means none active
	uint8_t numActivePowerups;

	player players[NUM_PLAYERS];
	capturepoint cp[NUM_CAPTUREPOINTS]; 	//array for the capture point structs 
	powerup active_powerups[MAX_NUM_POWERUPS];	//basically we cycle through this and check if they have surpassed their lifespan and remove them
	//we can send this as a comma separated list, since it is dynamic legth, when parsing the string, we just wait for two }} next to each other to mark last one 
	
}map;

//{D:id:q0,q1,q2,q3:x_gesture,y_gesture,z_gesture:x_js,y_js:b_primary,b_js,b_3}

typedef struct updatepacket {
	uint8_t playerid; //between 0 and NUMPLAYERS
	int32_t q0;
	int32_t q1;
	int32_t q2;
	int32_t q3;
	int8_t x_gesture;
	int8_t y_gesture;
	int8_t z_gesture;
	int16_t y_rot;
	int8_t x_js;
	int8_t y_js;
	uint8_t b_primary_t;
	uint8_t b_js_t;
	uint8_t b_3_t;
}updatepacket;

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
												 uint8_t b_pri, uint8_t b_js, uint8_t b_3);

block_pos MakeBlockPos(uint8_t x, uint8_t y);
pixel_pos MakePixelPos(int16_t x, int16_t y);

void Make_Map(map* m);

powerup Make_Powerup(int8_t type, int8_t weapon_type, int16_t boost_amount, int16_t boost_time, int16_t hitbox_x, int16_t hitbox_y, pixel_pos loc, uint32_t lifetime, int16_t speed, uint32_t cooldown);

uint8_t Check_Blocked(int16_t newx, int16_t newy);
uint8_t Check_Slow(int16_t newx, int16_t newy);

#endif
