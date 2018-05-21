//***********************  ESP8266.h  ***********************
// Program written by:
// - Steven Prickett  steven.prickett@gmail.com
//
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
//*********************************************************
/* Modified by Jonathan Valvano
 Sept 19, 2015
 
 */
enum {SOFTAP, CLIENT};

#ifndef ESP8266_H
#define ESP8266_H

#define ESP8266_ENCRYPT_MODE_OPEN		 	      0
#define ESP8266_ENCRYPT_MODE_WEP		 	      1
#define ESP8266_ENCRYPT_MODE_WPA_PSK	 	    2
#define ESP8266_ENCRYPT_MODE_WPA2_PSK	 	    3
#define ESP8266_ENCRYPT_MODE_WPA_WPA2_PSK	  4
 
#define ESP8266_WIFI_MODE_CLIENT            1
#define ESP8266_WIFI_MODE_AP                2
#define ESP8266_WIFI_MODE_AP_AND_CLIENT     3

#define SERVER_PORT 80
#define SERVER_IP "192.168.4.1"
#define UDP_PORT 4445
#define SSID_NAME  "ESP"
#define PASSKEY    "password"

#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 1024
#define MAXTRY 10

//-------------------ESP8266_Init --------------
// initializes the module as a client
// Inputs: none
// Outputs: none
void ESP8266_Init(uint32_t baud, uint8_t mode);

//------------------- ESP8266_InitUART-------------------
// intializes uart and gpio needed to communicate with esp8266
// Configure UART1 for serial full duplex operation
// Inputs: baud rate (e.g., 115200 or 9600)
//         echo to UART0?
// Outputs: none
void ESP8266_InitUART(uint32_t baud, int echo);

int ESP8266_SetBaud(int baud);

//---------ESP8266_GetVersionNumber----------
// get status
// Input: none
// output: 1 if success, 0 if fail 
int ESP8266_GetVersionNumber(void);

//----------ESP8266_Reset------------
// resets the esp8266 module
// input:  none
// output: 1 if success, 0 if fail
int ESP8266_Reset(void);

//---------ESP8266_SetWifiMode----------
// configures the esp8266 to operate as a wifi client, access point, or both
// Input: mode accepts ESP8266_WIFI_MODE constants
// output: 1 if success, 0 if fail 
int ESP8266_SetWifiMode(uint8_t mode);

//---------ESP8266_SetConnectionMux----------
// enables the esp8266 connection mux, required for starting tcp server
// Input: 0 (single) or 1 (multiple)
// output: 1 if success, 0 if fail 
int ESP8266_SetConnectionMux(uint8_t enabled);

//---------ESP8266_CloseTCPConnection----------
// Close TCP connection 
// Input: none
// output: 1 if success, 0 if fail 
int ESP8266_CloseTCPConnection(void);

//---------ESP8266_DisableServer----------
// disables tcp server
// Input: none 
// output: 1 if success, 0 if fail 
int ESP8266_DisableServer(void);

//----------ESP8266_JoinAccessPoint------------
// joins a wifi access point using specified ssid and password
// input:  SSID and PASSWORD
// output: 1 if success, 0 if fail
int ESP8266_JoinAccessPoint(const char* ssid, const char* password);

//---------ESP8266_ListAccessPoints----------
// lists available wifi access points
// Input: none
// output: 1 if success, 0 if fail 
int ESP8266_ListAccessPoints(void);

//----------ESP8266_ConfigureAccessPoint------------
// configures esp8266 wifi access point settings
// input:  SSID, Password, channel, security
// output: 1 if success, 0 if fail
int ESP8266_ConfigureAccessPoint(const char* ssid, const char* password, uint8_t channel, uint8_t encryptMode);

//---------ESP8266_GetIPAddress----------
// Get local IP address
// Input: none
// output: 1 if success, 0 if fail 
int ESP8266_GetIPAddress(void);

//---------ESP8266_MakeTCPConnection----------
// Establish TCP connection 
// Input: IP address or web page as a string
// output: 1 if success, 0 if fail 
int ESP8266_MakeTCPConnection(char *IPaddress);

//---------ESP8266_SendTCP----------
// Send a TCP packet to server 
// Input: TCP payload to send
// output: 1 if success, 0 if fail 
int ESP8266_SendTCP(char* fetch);

//---------ESP8266_SetDataTransmissionMode----------
// set data transmission mode
// Input: 0 not data mode, 1 data mode; return "Link is builded" 
// output: 1 if success, 0 if fail 
int ESP8266_SetDataTransmissionMode(uint8_t mode);
  
//---------ESP8266_GetStatus----------
// get status
// Input: none
// output: 1 if success, 0 if fail 
int ESP8266_GetStatus(void);

//--------ESP8266_EnableRXInterrupt--------
// - enables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_EnableRXInterrupt(void);

//--------ESP8266_DisableRXInterrupt--------
// - disables uart rx interrupt
// Inputs: none
// Outputs: none
void ESP8266_DisableRXInterrupt(void);

//--------ESP8266_PrintChar--------
// prints a character to the esp8226 via uart
// Inputs: character to transmit
// Outputs: none
// busy-wait synchronization
void ESP8266_PrintChar(char input);

// ----------ESP8266_QuitAccessPoint-------------
// - disconnects from currently connected wifi access point
// Inputs: none
// Outputs: 1 if success, 0 if fail 
int ESP8266_QuitAccessPoint(void);

//************the following are not tested********
void ESP8266_SetServerTimeout(uint16_t timeout);
void ESP8266_EnableServer(uint16_t port);

// serves a page via the ESP8266
void HTTP_ServePage(const char* body);
// added to code
int ESP8266_GetClients(void);
//--------- ESP8266_SetDHCP ----------
// Sets the DHCP of the esp
// Input: mode - dhcp mode (check data sheet)
//   			en - 1 for dhcp on, 0 for dhcp off
// output: 1 if success, 0 if fail 
int ESP8266_SetDHCP(uint8_t mode, uint8_t en);
//---------ESP8266_MakeUDPConnection----------
// Establish UDP connection 
// Input: IP address - ip address or string of web address
//				serverPort - server port number to connect to
//				UDPport - UDP port number to connect to
// output: 1 if success, 0 if fail 
int ESP8266_MakeUDPConnection(char *IPaddress, uint32_t serverPort, uint32_t UDPport);
//---------ESP8266_SetStationIP----------
// Sets the soft AP station IP address
// Input: IP address or web page as a string
// output: 1 if success, 0 if fail 
int ESP8266_SetStationIP(char *IPAddress);
//---------ESP8266_SendUDP----------
// Send a UDP packet to server (not tested)
// Input: UDP payload to send
// output: 1 if success, 0 if fail 
int ESP8266_SendUDP(char* fetch);

/*************** Get_LatestMessage *****************/
/* 	receives the latest message sent to the esp denoted
		with a +IPD in the beginning; max 1024 bytes
		Input: buffer - buffer that the latest message will be sent to
		Output: none
*/
void Get_LatestMessage(char* buffer);

/*************** Get_LatestMessageLength *****************/
/* 	returns the length of the latest message
		Input: none
		Output: latest message length
*/
uint16_t Get_LatestMessageLength(void);


#endif
