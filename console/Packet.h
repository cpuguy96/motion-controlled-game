
#ifndef Packet_h
#define Packet_h

/***************** Set_Latest_Packet ****************
	Sets the player data/message information based on packet sent
	Inputs: buffer - packet string
	Outputs: 1 if parsing succeded in updating player information, 0 if failed
*/
uint8_t Set_Latest_Packet(char buffer[BUFFER_SIZE]);
/***************** Get_Player_Data ****************
	Returns an updatepacket struct containing data of specified player 
	Inputs: player - player number
	Outputs: updatepacked contain player information
*/
updatepacket Get_Player_Data(uint8_t player);
/***************** Get_Player_Message ****************
	Returns a string containing message sent by a specified player 
	Inputs: player - player number
					buffer - buffer where message will be returned 
	Outputs: none
*/
void Get_Player_Message(uint8_t player,  char buffer[BUFFER_SIZE]);
/***************** Get_Latest_Packet_Type ****************
	Returns latest type of packet recieved
	Inputs: player - player number
	Outputs: -1 if no packet received yet, 0 if message packet received, player id + 1 if data received
*/
int8_t Get_Latest_Packet_Type(void);
/***************** Get_Latest_Updated_Player ****************
	Returns time of last recieved information from specified player
	Inputs: player - player id
	Outputs: time last recieved information from player
*/
uint8_t Get_Latest_Updated_Player(void);
/***************** Print_Player_Packet ****************
	Prints player packet information to UART
	Inputs: player - player id
	Outputs: none
*/
void Print_Player_Packet(uint8_t player);
/***************** Print_Latest_Packet ****************
	Prints the information from the latest packet recieved
	Inputs: none
	Outputs: none
*/
void Print_Latest_Packet(void);
/***************** Packet_Read_Init ****************
	Initializes the esp packet reader
	Inputs: none
	Outputs: none
*/
void Packet_Read_Init(void);
/***************** Get_Latest_Updated_Time ****************
	Returns time of last recieved information from specified player
	Inputs: player - player id
	Outputs: time last recieved information from player
*/
uint32_t Get_Last_Update_Time(uint8_t player);
#endif
