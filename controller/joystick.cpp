#include "joystick.h"


joystick::joystick(int _x_enable_pin, int _y_enable_pin, int _adc_pin, int _button_pin) {
  x_enable_pin = _x_enable_pin;
  y_enable_pin = _y_enable_pin;
  adc_pin = _adc_pin;
  button_pin = _button_pin;

  pinMode(x_enable_pin, OUTPUT);
  pinMode(y_enable_pin, OUTPUT);

  jButton = new button(button_pin);
}



int joystick::getX() {
  return x;
}

int joystick::getY() {
  return y;
}



//regions: -3, -2, -1, 0, 1, 2, 3
int joystick::getRegion(int adc_value) {
  if (adc_value > (REGION_MIDPOINT + REGION_0_OFF) && adc_value < (REGION_MIDPOINT + REGION_1_OFF)) {
    return 1;
  }
  else if (adc_value > (REGION_MIDPOINT - REGION_1_OFF) && adc_value < (REGION_MIDPOINT - REGION_0_OFF)) {
    return -1;
  }
  else if (adc_value >= (REGION_MIDPOINT + REGION_1_OFF) && adc_value < (REGION_MIDPOINT + REGION_2_OFF)) {
    return 2;
  }
  else if (adc_value >= (REGION_MIDPOINT - REGION_2_OFF) && adc_value < (REGION_MIDPOINT - REGION_1_OFF)) {
    return -2;
  }
  else if (adc_value >= (REGION_MIDPOINT + REGION_2_OFF)) {
    return 3;
  }
  else if (adc_value <= (REGION_MIDPOINT - REGION_2_OFF)) {
    return -3;
  }
  else {
    return 0;
  }
}

int joystick::getXRegion() {
  return getRegion(scaleX());
}

int joystick::getYRegion() {
  return getRegion(scaleY());
}





//update x value and y value from adc
void joystick::updateXY() {
  updateX();
  updateY();
}

//get most recently updated value for x, assumes both are currently low
int joystick::updateX() {
  digitalWrite(x_enable_pin, HIGH);   //setting pin high then low is in total about 7 microseconds
  delay(DELAY_BETWEEN_AXES);
  x = (analogRead(adc_pin));          //100 microseconds for adc
  digitalWrite(x_enable_pin, LOW);
  //delay(DELAY_BETWEEN_AXES);
  return x;
}

//get most recently updated value for y, assumes both are currently low
int joystick::updateY() {
  digitalWrite(y_enable_pin, HIGH);
  delay(DELAY_BETWEEN_AXES); 
  y = (analogRead(adc_pin));
  digitalWrite(y_enable_pin, LOW);
  delay(DELAY_BETWEEN_AXES);
  return y;
}






//get values for xmin, xmax, ymin, ymax, xcenter, ycenter
int calibrationTimeMinMaxS = 6;   //calibration time in seconds, for min/max
int numDataPointsMidpoint = 20;     //number of samples for midpoint
void joystick::calibrate() {
  Serial.println("Calibration Routine: 2 parts. Find center, find min/max.");

  //calib[6]: xmin, xmax, ymin, ymax, xcenter, ycenter
  calib[0] = 1000;
  calib[1] = 0;
  calib[2] = 1000;
  calib[3] = 0;
  calib[4] = 500;
  calib[5] = 500;

  //Part 1: Find Midpoint
  Serial.println("Part 1 Instructions: Leave the joystick at rest position.");
  Serial.println("Click the button to begin");
  while(!getButton()->buttonCheck()) { delay(500); }
  Serial.println("Starting Calibration in 1s");
  delay(1000);
  
  int xSum = 0;
  int ySum = 0;
  for (int i = 0; i < numDataPointsMidpoint; i++) {
    updateXY();
    xSum += x;
    ySum += y;
    delay(50);
  }
  calib[4] = xSum / numDataPointsMidpoint;
  calib[5] = ySum / numDataPointsMidpoint;

  Serial.println("Finished Part 1");
  delay(500);
  
  //Part 2: Find Min Max
  Serial.println("Part 2 Instructions: Roll the joystick in full range of motion, reach the min and max of each axis.");
  Serial.println("Click the button to begin");
  while(!getButton()->buttonCheck()) { delay(500); }
  Serial.println("Starting Calibration in 1s");
  delay(1000);

  long endTime = millis() + calibrationTimeMinMaxS * 1000;
  while(millis() < endTime) {
    updateXY();
    if (x < calib[0]) {
      calib[0] = x;
    }
    if (x > calib[1]) {
      calib[1] = x;
    }
    if (y < calib[2]) {
      calib[2] = y;
    }
    if (y > calib[3]) {
      calib[3] = y;
    }
    delay(50);
  }

  Serial.println("Finished Part 2");
  delay(500);

  Serial.println(calibrationToString());
  delay(2000);
}

String joystick::calibrationToString() {
  //calib[6]: xmin, xmax, ymin, ymax, xcenter, ycenter
  String s = "Calibration";
  s += " x_min:" + String(calib[0]);
  s += " x_max:" + String(calib[1]);
  s += " y_min:" + String(calib[2]);
  s += " y_max:" + String(calib[3]);
  s += " x_center:" + String(calib[4]);
  s += " y_center:" + String(calib[5]);
  return s;
}

int joystick::scaleX() {
  int scaledX = x;
  if (x < calib[4]) {
    int xmindiff = calib[4] - calib[0];
    scaledX = 500 * (double(x) / double(xmindiff));
  }
  else if (x > calib[4]) {
    int xmaxdiff = calib[1] - calib[4];
    scaledX = 500 + 500 * (double(x - calib[4]) / double(xmaxdiff));
  }
  else {
    scaledX = 500;
  }
  return scaledX;
}

int joystick::scaleY() {
  int scaledY = y;
  if (y < calib[5]) {
    int ymindiff = calib[5] - calib[2];
    scaledY = 500 * (double(y) / double(ymindiff));
  }
  else if (y > calib[5]) {
    int ymaxdiff = calib[3] - calib[5];
    scaledY = 500 + 500 * (double(y - calib[5]) / double(ymaxdiff));
  }
  else {
    scaledY = 500;
  }
  return scaledY;
}


button * joystick::getButton() {
  return jButton;
}




String joystick::toString() {
  String s = "x: " + String(x) + "\ty: " + String(y) + "\t" + String(getButton()->toString()) + "\n";
  s += "x: " + String(getXRegion()) + "\ty: " + String(getYRegion()) + "\n";
  s += "x scaled: " + String(scaleX()) + "\ty scaled: " + String(scaleY());
  return s;
}

String joystick::packetString() {
  return String((int)getXRegion()) + "," + String((int)getYRegion());
}

