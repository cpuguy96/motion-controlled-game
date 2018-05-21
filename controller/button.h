#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

/* 
 * negative logic buttons for gpio 0-14, positive logic for gpio16 (16 only has a pull down resistor)
 *  
 * button pin: 
 *    for interrupts to work, Arduino requires the attachInterrupt call and the ISR function to be in the main INO file 
 *    create function wrapper joystickButtonISR in main that calls this joystick's joystickButtonISR
 *    in setup: attachInterrupt(digitalPinToInterrupt(button_pin), buttonISR, FALLING);
 *    FALLING for negative logic, RISING for positive logic
 */
   

class button {

public:

  button(int _button_pin);

  boolean buttonCheck();      //check if button has been pressed since last time checked
  boolean getButtonCurrent();
  boolean getButtonPressed();
  
  void buttonISR();           //routine for main file to call in the ISR for the button

  String toString();          //get all data as a string. adc raw values, scaled values, regions
  String packetString();      //for the udp packet, something simple to send

private:
  int button_pin;

  //flag is true when the button is pressed, stays true until buttonCheck is called
  volatile boolean buttonPressed = false;
  volatile boolean buttonCurrent = false;
};

#endif
