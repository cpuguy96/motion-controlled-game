#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>
#include "button.h"

/* class to run 2 axis potentiometer joystick from 1 adc
 *  
 * Wiring:
 * Both positive power on potentiometers connected to GPIO pins, interchanged on each read
 * Potentiometer output pins connected to ADC input with diodes
 * 10k pulldown resistor on ADC input
 *
 *  
 * button pin: 
 *    negative logic button
 *    for interrupts to work, Arduino requires the attachInterrupt call and the ISR function to be in the main INO file 
 *    create function wrapper joystickButtonISR in main that calls this joystick's getButton()->buttonISR()
 *    in setup: attachInterrupt(digitalPinToInterrupt(button_pin), joystickButtonISR, FALLING);
 */

//the joystick will take (DELAY_BETWEEN_AXES * 4) seconds to use updateXY()
#define DELAY_BETWEEN_AXES 5

//values from 0-1000
//regions: -3, -2, -1, 0, 1, 2, 3
#define REGION_MIDPOINT 500
#define REGION_0_OFF    50   //distance from midpoint for region 0
#define REGION_1_OFF    250   //distance from midpoint for region 1
#define REGION_2_OFF    350   //distance from midpoint for region 1

class joystick {

public:

  joystick(int _x_enable_pin, int _y_enable_pin, int _adc_pin, int _button_pin);

  //update x value and y value from adc
  void updateXY(); 
  int updateX();
  int updateY();
  
  //get most recently updated values for x, y, assumes both are currently low
  int getX();
  int getY();

  //regions: -3, -2, -1, 0, 1, 2, 3
  //uses scaled values between 0 and 1000, 500 is center at rest position
  int getXRegion();
  int getYRegion();
  int getRegion(int adc_value);

  //get values for calib[] {xmin, xmax, ymin, ymax, xcenter, ycenter}
  void calibrate();

  button * getButton();     //access button methods

  String toString();                //get all data as a string. adc raw values, scaled values, regions
  String calibrationToString();     //get calibration data as string
  String packetString();            //for the udp packet, something simple to send

private:
  int x_enable_pin, y_enable_pin, adc_pin, button_pin;

  button * jButton;

  //most recent ADC values for x and y axis
  int x, y;
  
  //xmin, xmax, ymin, ymax, xcenter, ycenter
  int calib[6] = {0, 886, 1, 902, 313, 342};      //default data from one calibration
  

  //use calibration values to output on a balanced scale between 0 and 1000
  int scaleX();
  int scaleY();
};

#endif
