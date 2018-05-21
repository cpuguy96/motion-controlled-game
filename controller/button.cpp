#include "button.h"

button::button(int _button_pin) {
  button_pin = _button_pin;

  if (button_pin == 16) {
    pinMode(button_pin, INPUT_PULLDOWN_16); 
  }
  else if ((button_pin > 0 && button_pin < 6) || (button_pin > 11 && button_pin < 14)) {
    pinMode(button_pin, INPUT_PULLUP);  
  }
  else {
    Serial.println("Error, invalid button gpio pin: " + String(button_pin));
  }
}

boolean button::getButtonPressed() {
  return buttonPressed;
}

boolean button::getButtonCurrent() {
  return buttonCurrent;
}

boolean button::buttonCheck() {
  boolean check = buttonPressed;
  buttonPressed = false;
  return check;
}



void button::buttonISR() {
  int val = digitalRead(button_pin);
  if (val) {
    buttonCurrent = false;
  }
  else {
    buttonPressed = true;    
    buttonCurrent = true;
  }
}


String button::toString() {
  return "button: " + String((getButtonCurrent() || buttonCheck()));
}

String button::packetString() {
  return (getButtonCurrent() || buttonCheck())?"1":"0";
}

