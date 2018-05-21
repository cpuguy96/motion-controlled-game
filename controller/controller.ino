//Game Controller on ESP8266 12E
//Interfaces MPU9255, a 2-axis potentiometer joystick, 3 buttons
//Sends the information in a UDP, TCP, or serial packet


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include "gamecontroller.h"

#define AP_NAME "ESP"
#define AP_PASS "password"

//board: esp 12-e
//serial: baud=115200, both nl & cr

unsigned long previousMillis = 0;
const long interval = 20;

gamecontroller * gc;

Ticker ticker;    //timer interrupt class
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on





void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  setupWifi();
  setupGameController();

}

void setupWifi() {
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);

  WiFi.begin(AP_NAME, AP_PASS);

  Serial.print("Wait for WiFi... ");

  while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("WiFi connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

}

void setupGameController() {
  gc = new gamecontroller(0);
  
  gc->getImu()->calibrate();    //currently need to do this before interrupt update set, but that can be changed
  
  ticker.attach_ms(TIMER_UPDATE_MS, timerUpdateISR);    //timer interrupt update
  attachInterrupt(digitalPinToInterrupt(B_PRIMARY_PIN), b_primary_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_JS_PIN), b_js_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(B_3_PIN), b_3_ISR, CHANGE);

  Serial.println("[DEBUG] Controller init finished");

  //eventually need to set IMU initial orientation, maybe with button press before game starts
  //gc->getJs()->calibrate();

}







void loop() {
  //This stuff is for the Visualizer 
  //Serial.println(gc->getImu()->quaternionVisualizerString());     //for serial Processing visualizer
  //udpOut(gc->getImu()->quaternionVisualizerString());

  //This stuff is for the game
  unsigned long currentMillis = millis();
  boolean setInitPos = gc->getB_3()->getButtonPressed();
  
  if (currentMillis - previousMillis >= interval) {    
    //Serial.println(String("Update Interval: " + String(currentMillis - previousMillis)));

    previousMillis = currentMillis;
    String output = gc->packetStringData();
    udpOut(output);
    Serial.println(output);
  
    if (setInitPos) {
      gc->getImu()->setInitialOrientation();
    }
    
  }
}








//ISR's

void timerUpdateISR() {
  gc->timerUpdateISR();
}

//arduino requires the ISR to be in the main INO file, so use a wrapper
void b_js_ISR() {
  gc->getB_js()->buttonISR();
}
void b_primary_ISR() {
  gc->getB_primary()->buttonISR();
}
void b_3_ISR() {
  gc->getB_3()->buttonISR();
}












//WIFI

void udpOut(String output) {
  const uint16_t port = 4445;
  const char * host = "192.168.4.1"; // ip or dns

  char charBuf[50];
  output.toCharArray(charBuf, 50) ;
  
  Udp.beginPacket(host, port);
  Udp.write(charBuf);
  Udp.endPacket();
}

void tcpOut(String output) {
  const uint16_t port = 80;
  const char * host = "192.168.4.1"; // ip or dns

  Serial.println("connecting to " + String(host));

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
      Serial.println("connection failed");
      Serial.println("wait 1 sec...");
      delay(1000);
      return;
  }

  // This will send the request to the server
  Serial.print("output: ");
  client.println(output);

  //read back one line from server
  //String line = client.readStringUntil('\r');
  //Serial.println(line);

  Serial.println("closing connection");
  client.stop();

  //delay(1000);
}

