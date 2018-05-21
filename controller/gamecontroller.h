#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

/* Controller for game
 * IMU, Joystick, 3 buttons, and an LED
 *
 * In Main, set up timer update for IMU quaternions, and interrupts for the buttons
 *
 */

#include <Arduino.h>
#include "button.h"
#include "joystick.h"
#include "mpu9255.h"

//PINS
#define B_PRIMARY_PIN 12	//d6 
#define B_3_PIN 2			    //d4        //note: if you press this button during start up it will cause a boot error
#define ADC_PIN A0   		  // select the input pin for the potentiometer
#define X_EN_PIN 16      	// enable reading sensor A, d0
#define Y_EN_PIN 14      	// enable reading sensor B, d5
#define B_JS_PIN 13       // push button on joystick, negative logic, d7



class gamecontroller {
public:
	gamecontroller(int controller_id);

	int getId();

	mpu9255 * getImu();
	joystick * getJs();
	button * getB_primary();
	button * getB_js();
	button * getB_3();

  void timerUpdateISR();

	String toString();
	String packetStringData();		//{D:id:q0,q1,q2,q3:x_gesture,y_gesture,z_gesture:y_rot:x_js,y_js:b_primary,b_js,b_3}
	String packetStringMssg(String mssg);		//{M:id:message}


private:
	int id;

	mpu9255 * imu;
	joystick * js;
	button * b_primary;
	button * b_js;
	button * b_3;
	//LED driver

};


#endif
