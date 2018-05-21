
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../inc/tm4c123gh6pm.h"
#include "gamemodel.h"
#include "esp8266.h"
#include "Packet.h"
#include "time.h"

static updatepacket LatestDataPacket[NUM_PLAYERS]; 			// last data packet sent from each controller
static char LatestMessage[NUM_PLAYERS][BUFFER_SIZE];		// last message sent from each controller
static uint8_t ParseSuccess;														// maybe used for debugging
static int8_t LatestPacketType; 												// -1 for unknown, 0 for message, 1-2 for players
static int8_t LatestUpdatedPlayer;											// last player updated 
static uint32_t LastUpdateTime[NUM_PLAYERS];						// last update time of player

/***************** Packet_Read_Init ****************
	Initializes the esp packet reader
	Inputs: none
	Outputs: none
*/
void Packet_Read_Init(void){
	uint8_t index;
	for(index = 0; index < NUM_PLAYERS; index++){
		LatestDataPacket[index] = Make_Packet(0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		strcpy(LatestMessage[index], "");
		
	}
	LatestPacketType = -1;
	LatestUpdatedPlayer = -1;	
}
/***************** Set_Latest_Packet ****************
	Sets the player data/message information based on packet sent
	Inputs: buffer - packet string
	Outputs: 1 if parsing succeded in updating player information, 0 if failed
*/
uint8_t Set_Latest_Packet(char buffer[BUFFER_SIZE]){
	uint16_t length = strlen(buffer);
	if(buffer[0] != '{' || buffer[length - 1] != '}'){		// checks if {D/M:...} format was received 
		//printf("error parsing message\n\r");							// error message if parsing failed
		ParseSuccess = FALSE;
		LatestPacketType = -1;															
		return ParseSuccess;
	}
	char* data;
	uint8_t playerid, b_pri, b_js, b_3;
	int8_t x_gesture, y_gesture, z_gesture, x_js, y_js;
	int32_t q0, q1, q2, q3;
	int16_t y_rot;
	data = strtok(buffer, PACKET_DELS);  // gets what type of packet sent; D for data or M for message
	char packetType = data[0];
	data = strtok(NULL, PACKET_DELS);
	playerid = (uint8_t)atoi(data);
	if(playerid >= NUM_PLAYERS){
		ParseSuccess = FALSE;
		LatestPacketType = -1;															
		return ParseSuccess;
	}
	switch(packetType){
	case 'D':
		data = strtok(NULL, PACKET_DELS);
		q0 = (int32_t)atoi(data);
	
		data = strtok(NULL, PACKET_DELS);
		q1 = (int32_t)atoi(data);
	
		data = strtok(NULL, PACKET_DELS);
		q2 = (int32_t)atoi(data);
	
		data = strtok(NULL, PACKET_DELS);
		q3 = (int32_t)atoi(data);
	
		data = strtok(NULL, PACKET_DELS);
		x_gesture = (int8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		y_gesture = (int8_t)atoi(data);
	
		data = strtok(NULL, PACKET_DELS);
		z_gesture = (int8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		y_rot = (int16_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		x_js = (int8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		y_js = (int8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		b_pri = (uint8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		b_js = (uint8_t)atoi(data);
		
		data = strtok(NULL, PACKET_DELS);
		b_3 = (uint8_t)atoi(data);
		
		LatestUpdatedPlayer = playerid;
		LatestDataPacket[playerid] = Make_Packet(playerid, q0, q1, q2, q3, x_gesture, y_gesture, z_gesture, y_rot, x_js, y_js, b_pri, b_js, b_3);
		LatestPacketType = playerid + 1;					// latest packet is playerid + 1 for toString function
		LastUpdateTime[playerid] = Get_Epoch();
		break;
	case 'M':
		data = strtok(NULL, PACKET_DELS);
		playerid = (uint8_t)atoi(data);
		memset(LatestMessage[playerid], 0, BUFFER_SIZE * sizeof(char));
		data = strtok(NULL, PACKET_DELS);
		strcpy(LatestMessage[playerid], data);			// message cannot contain a '{', ':', ',', or ,},
		LatestUpdatedPlayer = playerid;
		LatestPacketType = 0;	
		LastUpdateTime[playerid] = Get_Epoch();
		break;
	default:
		ParseSuccess = FALSE;
		LatestPacketType = -1;
		return ParseSuccess;
	}
	ParseSuccess = TRUE;
	return ParseSuccess;
}
/***************** Get_Player_Data ****************
	Returns an updatepacket struct containing data of specified player 
	Inputs: player - player number
	Outputs: updatepacked contain player information
*/
updatepacket Get_Player_Data(uint8_t player){
	return LatestDataPacket[player];
}
/***************** Get_Player_Message ****************
	Returns a string containing message sent by a specified player 
	Inputs: player - player number
					buffer - buffer where message will be returned 
	Outputs: none
*/
void Get_Player_Message(uint8_t player, char buffer[BUFFER_SIZE]){
	strcpy(buffer, LatestMessage[player]);
}
/***************** Get_Latest_Packet_Type ****************
	Returns latest type of packet recieved
	Inputs: player - player number
	Outputs: -1 if no packet received yet, 0 if message packet received, player id + 1 if data received
*/
int8_t Get_Latest_Packet_Type(void){
	return LatestPacketType;
}
/***************** Get_Latest_Updated_Player ****************
	Returns player id of latest updated player
	Inputs: none
	Outputs: player id of latest updated player
*/
uint8_t Get_Latest_Updated_Player(void){
	return LatestUpdatedPlayer;
}
/***************** Get_Latest_Updated_Time ****************
	Returns time of last recieved information from specified player
	Inputs: player - player id
	Outputs: time last recieved information from player
*/
uint32_t Get_Last_Update_Time(uint8_t player){
	return LastUpdateTime[player];
}
/***************** Print_Player_Packet ****************
	Prints player packet information to UART
	Inputs: player - player id
	Outputs: none
*/
void Print_Player_Packet(uint8_t player){
	player --; // first player index is 0
	printf("\n\r---------\n\r");
	printf("Player:\t%d\n\r", LatestDataPacket[player].playerid);
	printf("Q0:\t%d\n\r", LatestDataPacket[player].q0);
	printf("Q1:\t%d\n\r", LatestDataPacket[player].q1);
	printf("Q2:\t%d\n\r", LatestDataPacket[player].q2);
	printf("Q3:\t%d\n\r", LatestDataPacket[player].q3);
	printf("X Gest:\t%d\n\r", LatestDataPacket[player].x_gesture);
	printf("Y Gest:\t%d\n\r", LatestDataPacket[player].y_gesture);
	printf("Z Gest:\t%d\n\r", LatestDataPacket[player].z_gesture);
	printf("Y Rot:\t%d\n\r", LatestDataPacket[player].y_rot);
	printf("X Joy:\t%d\n\r", LatestDataPacket[player].x_js);
	printf("Y Joy:\t%d\n\r", LatestDataPacket[player].y_js);
	printf("B Pri:\t%d\n\r", LatestDataPacket[player].b_primary_t);
	printf("B Joy:\t%d\n\r", LatestDataPacket[player].b_js_t);
	printf("B 3:\t%d\n\r", LatestDataPacket[player].b_3_t);
	printf("---------\n\r");
}
/***************** Print_Latest_Packet ****************
	Prints the information from the latest packet recieved
	Inputs: none
	Outputs: none
*/
void Print_Latest_Packet(void){
	int8_t latestPlayerUpdate = Get_Latest_Updated_Player();
	switch(Get_Latest_Packet_Type()){
	case MESSAGE:
		if(latestPlayerUpdate == -1){
			printf("\n\r---------\n\rno message sent yet\n\r---------\n\r");
		}
		else{
			printf("\n\r---------\n\rPlayer %d: %s\n\r---------", latestPlayerUpdate, LatestMessage[latestPlayerUpdate]);
		}
		break;
	case PLAYER1:
		Print_Player_Packet(PLAYER1);
		break;
	case PLAYER2:
		Print_Player_Packet(PLAYER2);
		break;
	default:
		printf("\n\r---------\n\runknown packet type\n\r");
	}
}
