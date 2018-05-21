#include <Arduino.h>
//MPU9255 10-DOF IMU Class
//For EE445L Final Project at UT Austin ECE

#include <Wire.h>
#include "mpu9255.h"

mpu9255::mpu9255() {
  Wire.begin();
  
  //write registers to set up (datasheet)

	//make sure the imu is a mpu9255
	int id = i2cRegRead8(IMU_I2C_ID, MPU9255_WHO_AM_I);
	if (id != MPU9255_ID) {
    Serial.print("\nError, not the right kind of imu: ");
    Serial.println(id, HEX);
    while(1);
	}
  else {
    Serial.println("\nMPU9255 Identified");
  }


	//reset imu
	i2cRegWrite8(IMU_I2C_ID, PWR_MGMT_1, 0x80);
	delay(100);
	i2cRegWrite8(IMU_I2C_ID, PWR_MGMT_1, 0x00);	

	//gyro init
  i2cRegWrite8(IMU_I2C_ID, GYRO_CONFIG, 0x00);    //8khz sample rate
  i2cRegWrite8(IMU_I2C_ID, GYRO_LPF, 0x00);   //low pass filter

  //accel init
  i2cRegWrite8(IMU_I2C_ID, ACCEL_CONFIG, 0x00);
  i2cRegWrite8(IMU_I2C_ID, ACCEL_LPF, 0x00);    //low pass filter

  //sample rate SMPRT_DIV
  i2cRegWrite8(IMU_I2C_ID, SMPRT_DIV, 0x00);


  //compass init
  bypassOn();     //bypass on to access AK8963_I2C_ID, read compass sensitivities
  i2cRegWrite8(AK8963_I2C_ID, AK8963_CNTL, 0x00);   
  i2cRegWrite8(AK8963_I2C_ID, AK8963_CNTL, 0x0F); 
  compassSensitivity[X_AXIS] = signExtend(i2cRegRead8(AK8963_I2C_ID, AK8963_ASAX), 7); 
  compassSensitivity[Y_AXIS] = signExtend(i2cRegRead8(AK8963_I2C_ID, AK8963_ASAY), 7); 
  compassSensitivity[Z_AXIS] = signExtend(i2cRegRead8(AK8963_I2C_ID, AK8963_ASAZ), 7); 
  i2cRegWrite8(AK8963_I2C_ID, AK8963_CNTL, 0x00); 
  bypassOff();    //bypass off to access IMU_I2C_ID

  //configure slv0 (compass data regs)
  i2cRegWrite8(IMU_I2C_ID, I2C_MST_CTRL, 0x40);
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV0_ADDR, 0x80 | AK8963_I2C_ID);    //read mode, i2c id
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV0_REG, 0x02);                     //start register
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV0_CTRL, 0x88);                    //enabled, 8 bytes

  //configure slv1 (compass CNTL reg)
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV1_ADDR, AK8963_I2C_ID);           //write mode, i2c id
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV1_REG, AK8963_CNTL);              //start register
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV1_CTRL, 0x81);                    //enabled, 1 byte
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV1_DO, 0x02);                      //continuous compass reading mode (CNTL1)
  
  i2cRegWrite8(IMU_I2C_ID, I2C_MST_DELAY_CTRL, 0x03);               //enable slv0 slv1 sampling
  i2cRegWrite8(IMU_I2C_ID, I2C_SLV4_CTRL, 31);                      //compass sample rate (this control all slv rates)
  
  //enable accel and gyro
  i2cRegWrite8(IMU_I2C_ID, PWR_MGMT_1, 0x01);
  i2cRegWrite8(IMU_I2C_ID, PWR_MGMT_2, 0x00);

  
  //FIFO
  //i think this is set up right, there is a register to read from the fifo but im not sure how to know which data is coming in
  //i2cRegWrite8(IMU_I2C_ID, FIFO_EN, 0x79);    //enable writing Gyro, Accel, SLV0 to FIFO
  //i2cRegWrite8(IMU_I2C_ID, USER_CTRL, 0x44);  //enable and reset fifo


  Serial.println("\nMPU9255 Initialized");
}








//Accelerometer

void mpu9255::updateAccel() {
  int data[6] = {0};
  i2cRegRead(IMU_I2C_ID, ACCEL_XOUT_H, 6, data);
  for (int i = 0; i < 3; i++) {
    accel[i] = convertRawAccel(((data[2 * i] & 0x0FF) << 8) + (data[2 * i + 1] & 0x0FF), i);
  }
}

//update one axis of the Accel
double mpu9255::updateAccelAxis(int axis) {
  int data = 0;
  if (axis == X_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, ACCEL_XOUT_H, true);
  }
  else if (axis == Y_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, ACCEL_YOUT_H, true);
  }
  else if (axis == Z_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, ACCEL_ZOUT_H, true);
  }
  accel[axis] = convertRawAccel(data, axis);
  return accel[axis];
}

//Scaling mode set to [-2,2]. 16 bit register. 2^16 / 4 = 16384
double mpu9255::convertRawAccel(int data, int axis) {
  return double(signExtend(data, 15)) / 16384.0 - accelOffsets[axis];
}

double mpu9255::getAccelAxis(int axis) {
  return accel[axis];
}

//return direction enum, the axis that the current acceleration of the device is closest to, accounting for gravity and rotation
vec3 mpu9255::getAccelDirectionUser() {
  vec3 a = getAccelVec();

  //rotate the acceleration vector by the quaternion that represents the orientation of the device
  //this results in a vector that represents motion relative to the world, NESW, UD
  deviceToWorldSpace(a);
  //Serial.println(String("Acceleration Vector World Space: " + a.toString()));

  //negate world gravity
  vec3 grav = vec3(0, 0, 1);
  a -= grav;                  

  //rotate the acceleration vector by the inverse of the quaternion that represents the initial orientation
  //this results in a vector that represents motion relative to the user initial orientation
  worldToUserSpace(a);
  //Serial.println(String("Acceleration Vector User Space: " + a.toString()));

  return a;
}

double mpu9255::getAccelMag() {
  vec3 accelUserSpace = getAccelDirectionUser();
  return accelUserSpace.mag();
}








//Gyroscope

void mpu9255::updateGyro() {
  int data[6] = {0};
  i2cRegRead(IMU_I2C_ID, GYRO_XOUT_H, 6, data);
  for (int i = 0; i < 3; i++) {
    gyro[i] = convertRawGyro((((data[2 * i] & 0x0FF) << 8) + (data[2 * i + 1] & 0x0FF)), i);
  }
}

double mpu9255::updateGyroAxis(int axis) {
  int data = 0;
  if (axis == X_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, GYRO_XOUT_H, true);
  }
  else if (axis == Y_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, GYRO_YOUT_H, true);
  }
  else if (axis == Z_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, GYRO_ZOUT_H, true);
  }
  gyro[axis] = convertRawGyro(data, axis);
  return gyro[axis];
}

//Scaling mode set to [-250,250]. 16 bit register. 2^16 / 500 = 131
//note: readings were doing 60x the rotation, not sure why, so we divided by 60
double mpu9255::convertRawGyro(int data, int axis) {
  return double(signExtend(data, 15)) / (131.0 * 60) - gyroOffsets[axis];
}

double mpu9255::getGyroAxis(int axis) {
  return gyro[axis];
}










//Compass

void mpu9255::updateCompass() {
  int data[6] = {0};
  i2cRegRead(IMU_I2C_ID, GYRO_XOUT_H, 6, data);
  for (int i = 0; i < 3; i++) {
    compass[i] = convertRawCompass((((data[2 * i + 1] & 0x0FF) << 8) + (data[2 * i] & 0x0FF)), i);
  }
}

//the commented out portion is if we don't want to use slv0 for data, and instead use bypass mode
double mpu9255::updateCompassAxis(int axis) {
  int data = 0;
  if (axis == X_AXIS) {
    //bypassOn();
    //data = i2cRegRead16(AK8963_I2C_ID, AK8963_Z_L, false);
    //bypassOff();
    data = i2cRegRead16(IMU_I2C_ID, CMPS_XOUT_L, false);
  }
  else if (axis == Y_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, CMPS_YOUT_L, false);
  }
  else if (axis == Z_AXIS) {
    data = i2cRegRead16(IMU_I2C_ID, CMPS_ZOUT_L, false);
  }
  compass[axis] = convertRawCompass(data, axis);
  return compass[axis];
}

double mpu9255::convertRawCompass(int data, int axis) {
  data = double(signExtend(data, 15));
  return data * ( (((compassSensitivity[axis] - 128.0) * 0.5) / 128.0) + 1)  - compassOffsets[axis];
}

double mpu9255::getCompassAxis(int axis) {
  return compass[axis];
}






//Update All Sensors

//Read Accelerometer, Gyroscope, Compass in one burst
void mpu9255::update9Axes() {
  //updateGyro();
  //updateAccel();
  //updateCompass();

  //reading from 21 registers, x3b to x4f. 18 of these are sensor data, 3 not needed
  int data[21] = {0};
  i2cRegRead(IMU_I2C_ID, ACCEL_XOUT_H, 21, data);

  for (int i = 0; i < 3; i++) {
    accel[i] = convertRawAccel(((data[2 * i] & 0x0FF) << 8) + (data[2 * i + 1] & 0x0FF), i);
    gyro[i] = convertRawGyro((((data[8 + 2 * i] & 0x0FF) << 8) + (data[8 + 2 * i + 1] & 0x0FF)), i);    
    compass[i] = convertRawCompass((((data[15 + 2 * i + 1] & 0x0FF) << 8) + (data[15 + 2 * i] & 0x0FF)), i);
  }
}









//Vector stuff

vec3 mpu9255::getGyroVec() {
  return vec3(gyro[X_AXIS], gyro[Y_AXIS], gyro[Z_AXIS]);
}

vec3 mpu9255::getAccelVec() {
  return vec3(accel[X_AXIS], accel[Y_AXIS], accel[Z_AXIS]);
}

vec3 mpu9255::getCompassVec() {
  return vec3(compass[X_AXIS], compass[Y_AXIS], compass[Z_AXIS]);
}

//return direction enum for the axis that this vector is closest to, for a vector in user space
int mpu9255::getVectorUserDirection(vec3 v) {
  int maxAxis = v.getClosestAxis();

  bool pos = maxAxis > 0;
  maxAxis = abs(maxAxis) - 1;   //getClosestAxis returns (axis+1) so that it can be negative, otherwise +0 -0 would be a problem

  int dir = -1;  
  switch(maxAxis) {
    case X_AXIS: dir = pos?L:R; break;
    case Y_AXIS: dir = pos?B:F; break;
    case Z_AXIS: dir = pos?U:D; break;
    default: dir = -1;
  }

  return dir;
}

//return direction enum for the axis that this vector is closest to, for a vector in world space
int mpu9255::getVectorWorldDirection(vec3 v) {
  int maxAxis = v.getClosestAxis();

  bool pos = maxAxis > 0;
  maxAxis = abs(maxAxis) - 1;   //getClosestAxis returns (axis+1) so that it can be negative, otherwise +0 -0 would be a problem

  int dir = -1;
  switch(maxAxis) {
    case X_AXIS: dir = pos?N:S; break;
    case Y_AXIS: dir = pos?W:E; break;
    case Z_AXIS: dir = pos?U:D; break;
    default: dir = -1;
  }

  return dir;
}


void mpu9255::deviceToWorldSpace(vec3& v) {
  v.rotateByQuaternion(q0, q1, q2, q3);   //a is now the relative acceleration to world
}

void mpu9255::worldToDeviceSpace(vec3& v) {
  v.rotateByQuaternion(q0, -1 * q1, -1 * q2, -1 * q3);   //a is now the relative acceleration to world
}

void mpu9255::worldToUserSpace(vec3& v) {
  v.rotateByQuaternion(q0i, -1 * q1i,  -1 * q2i,  -1 * q3i);   //a is now the relative acceleration to user initial orientation
}

void mpu9255::userToWorldSpace(vec3& v) {
  v.rotateByQuaternion(q0i, q1i, q2i, q3i);   //a is now the relative acceleration to user initial orientation
}











//Calibration Routines
int numOrientationSamples = 5;
void mpu9255::setInitialOrientation() {     //TODO
  /* the quaternions sometimes require some movement to start reading consistent values, so this needs to happen once they even out
   * can't delay in here without messing up quat calculation freq timing, we can probably add a way to pause it
   * essentailly need a better way to even out the quat calculation inside of the initial calibration routine
   * maybe tell user to shake until we see significant changes in quat values, then we tell user to keep it still while we record the orientation
   */
  Serial.println("Calculating Initial Orientation");
  
  //delay(3000);

  q0i = 0.0f;
  q1i = 0.0f;
  q2i = 0.0f;
  q3i = 0.0f;

  for (int i = 0; i < numOrientationSamples; i++) {
    //update9Axes();
    //updateQuaternion();
    //Serial.println(dataToString()); 
    q0i += q0;
    q1i += q1;
    q2i += q2;
    q3i += q3;
    delay(50);
  }
  q0i /= numOrientationSamples;
  q1i /= numOrientationSamples;
  q2i /= numOrientationSamples;
  q3i /= numOrientationSamples;
  

  Serial.println(String("Initial Orientation:\tq0=" + String(q0i) + " q1=" + String(q1i) + " q2=" + String(q2i) + " q3=" + String(q3i) + "\n"));
}

//Calibration - while imu is still, point +z-axis up
void mpu9255::calibrate() {
  Serial.println("Starting Calibration - point +z-axis up");

  long startTime = millis();
  
  int numDataPoints = 100;
  vec3 sumG = vec3(0, 0, 0);
  vec3 sumA = vec3(0, 0, 0);
  vec3 sumC = vec3(0, 0, 0);

  for (int axis = 0; axis < 3; axis++) {
    gyroOffsets[axis] = 0;
    accelOffsets[axis] = 0;
    compassOffsets[axis] = 0;
  }

  for (int i = 0; i < numDataPoints; i++) {
    update9Axes();
    vec3 g = getGyroVec();
    vec3 a = getAccelVec();
    vec3 c = getCompassVec();
    sumG += g;
    sumA += a;
    sumC += c;
    delay(60);
  }

  for (int axis = 0; axis < 3; axis++) {
    gyroOffsets[axis] = sumG.getAxis(axis) / numDataPoints;
    accelOffsets[axis] = sumA.getAxis(axis) / numDataPoints;
    compassOffsets[axis] = sumC.getAxis(axis) / numDataPoints;
  }

  accelOffsets[Z_AXIS] -= 1;    //+z axis has gravity
  
  Serial.println(offsetsToString());
  
  long seconds = (millis() - startTime) / 1000;
  Serial.println("Calibration time: " + String(seconds) + "s");

  //the quaternions sometimes require some movement to start reading consistent values
  //set initial orientation
  //setInitialOrientation();
  
  Serial.println("Finished Calibration\n");
}

bool mpu9255::checkForGesture() {
  double aMag = vec3::absD(getAccelMag() - 1.0);
  if (aMag > SWIPE_TOLERANGE_MAG) {
    return true;
  }
  else {
    return false;
  }
}

int mpu9255::checkForGestureAxis(int axis) {
  vec3 accelUserSpace = getAccelDirectionUser();
  double aMag = vec3::absD(accelUserSpace.mag() - 1.0);
  if (aMag > SWIPE_TOLERANGE_MAG) {
    double theta = acos(vec3::absD(accelUserSpace.getAxis(axis)) / aMag);   //angle between magnitude and desired axis
    if (theta < SWIPE_TOLERANGE_ANGLE_R) {
      return (accelUserSpace.getAxis(axis) > 0)?1:-1;
    }
  }
  else {
    return 0;
  }
}

vec3 mpu9255::checkForGestureAxis() {   //TODO
  vec3 gestures = vec3();
  vec3 accelUserSpace = getAccelDirectionUser();
  double aMag = accelUserSpace.mag();
  double theta = 0;
 //Serial.println(String("angle tol: " + String(SWIPE_TOLERANGE_ANGLE_R) ));
  if (aMag > SWIPE_TOLERANGE_MAG) {
    for (int i = 0; i < 3; i++) {
      theta = acos(vec3::absD(accelUserSpace.getAxis(i)) / aMag);   //angle between magnitude and desired axis
      //Serial.println(String("axis:" + String(i) + " theta: " + String(theta) ));
      if (theta < SWIPE_TOLERANGE_ANGLE_R) {
        gestures.setAxis(i, (accelUserSpace.getAxis(i) > 0)?1:-1);
      }
    }
  }
  return gestures;
}

//for the controls of the game, direction of look for character
double mpu9255::getRotationAroundAxis(int axis) {
    double qval = 0;
    switch(axis) {
      case X_AXIS: qval = q1; break;
      case Y_AXIS: qval = q2; break;
      case Z_AXIS: qval = q3; break;
      default: qval = q2; break;
    }

    double theta = 2*acos(q0 / (sqrt(q0 * q0 + qval * qval))) * 180 / PI;
    theta = (qval < 0)?(-1 * theta):theta;

    theta *= -1;
    if (theta < 0 ) {
      theta *= -1;
    }
    else if (theta > 0) {
      theta = 360 - theta;
    }

    return theta;
}














//Printing

//make sure to update9Axes and updateQuaternion first
String mpu9255::dataToString() {  
  //Quaternions 
  String quatStr = String("Quaternions:\tq0=" + String(q0) + " q1=" + String(q1) + " q2=" + String(q2) + " q3=" + String(q3));
  
  //world and user orientation
  String worldOrientation = worldOrientationTestString();
  
  //acceleration in user orientation, check for gestures
  vec3 accelUserSpace = getAccelDirectionUser();
  double aMag = vec3::absD(accelUserSpace.mag() - 1.0);
  int dir = getVectorUserDirection(accelUserSpace);   //axis that the swipe is closest to
  String accelStrDir = String("Acceleration dir:" + String(dirToChar(dir)) + " magnitude:" + String(aMag));
  //String swipeStr = "Swipe: none";
  //if (checkForGestureAxis()) { swipeStr = String("Swipe: " + String(dirToChar(dir))); }
  vec3 gestures = checkForGestureAxis();
  String swipeStr = String("Swipe: " + gestures.toString());

  //sensor data
  String accelStr = String("Accelerometer Sensor Space(g): \tX=" + getAccelVec().toString());
  String accelStrUser = String("Acceleration User Space: " + accelUserSpace.toString());
  String gyroStr = String("Gyroscope (deg/sec): \tX=" + getGyroVec().toString());
  String compassStr = String("Compass (uT): \tX=" + getCompassVec().toString());
  
  return String(quatStr + "\n" + worldOrientation + "\n" + gyroStr + "\n" + compassStr + "\n" + accelStr + "\n" + accelStrUser + "\n" + accelStrDir + "\n" + swipeStr + "\n");
}

String mpu9255::quaternionVisualizerString() {
  return String("{" + String(q0) + "}{" + String(q1) + "}{" + String(q2) + "}{" + String(q3) + "}x");
}

String mpu9255::packetString() {
  vec3 gestures = checkForGestureAxis();
  double thetaAroundY = getRotationAroundAxis(Y_AXIS);
  
  String s = String(String(int(q0 * 1000)) + "," + String(int(q1 * 1000)) + "," + String(int(q2 * 1000)) + "," + String(int(q3 * 1000)) + ":");
  s += String(String((int)gestures.getAxis(X_AXIS)) + "," + String((int)gestures.getAxis(Y_AXIS)) + "," + String((int)gestures.getAxis(Z_AXIS)) + ":");
  s+= String(String((int)thetaAroundY));
  return s;
}

String mpu9255::offsetsToString() {
  String gyroStr = String("Gyro Offsets: Xo" + String(gyroOffsets[0]) + " Yo=" + String(gyroOffsets[1]) + " Zo=" + String(gyroOffsets[2]));
  String accelStr = String("Accel Offsets: Xo" + String(accelOffsets[0]) + " Yo=" + String(accelOffsets[1]) + " Zo=" + String(accelOffsets[2]));
  String compassStr = String("Compass Offsets: Xo" + String(compassOffsets[0]) + " Yo=" + String(compassOffsets[1]) + " Zo=" + String(compassOffsets[2]));
  return String(gyroStr + "\n" + accelStr + "\n" + compassStr + "\n");
}

//Directions enum
char mpu9255::dirToChar(int dir) {
  char s = 'I';
  switch(dir) {
    case F: s = 'F'; break;
    case B: s = 'B'; break;
    case U: s = 'U'; break;
    case D: s = 'D'; break;
    case L: s = 'L'; break;
    case R: s = 'R'; break;
    case N: s = 'N'; break;
    case S: s = 'S'; break;
    case E: s = 'E'; break;
    case W: s = 'W'; break;
    default:  s = 'I'; break;
  }
  return s;
}

String mpu9255::worldOrientationTestString() {
  vec3 fwd = vec3(0, -1, 0);
  userToWorldSpace(fwd);
  worldToDeviceSpace(fwd);
  double thetaToFwd = fwd.getAngleToAxis(Y_AXIS);

  vec3 up = vec3(0, 0, 1);
  userToWorldSpace(up);
  worldToDeviceSpace(up);
  double thetaToUp = up.getAngleToAxis(Z_AXIS);

  double thetaAroundY = getRotationAroundAxis(Y_AXIS);
  
  //vec3 north = vec3(1, 0, 0);
  //worldToDeviceSpace(north);
  String s = String("Sensor axis in: user fwd dir:" + String(fwd.closestAxisToString()) + ", user up dir:" + String(up.closestAxisToString()));
  s += String("\nAngle to: user fwd dir:" + String(thetaToFwd) + ", user up dir:" + String(thetaToUp));
  s += String("\nAngle around Y: " + String(thetaAroundY));
  return s;
}















//I2C Register Access

//data must be an array length numBytes
void mpu9255::i2cRegRead(int i2cID, int reg, int numBytes, int* data) {
  Wire.beginTransmission(i2cID);   // select device with "beginTransmission()"
  Wire.write(reg & 0x0FF);              // select starting register with "write()"
  Wire.endTransmission();               // end write operation, as we just wanted to select the starting register
  
  Wire.requestFrom(i2cID, numBytes);      // select number of bytes to get from the device (2 bytes in this case)
  for (int i = 0; i < numBytes; i++) {
    data[i] = Wire.read();
  }
}

//if highBitFirst is true, msb address should be less than lsb address
int mpu9255::i2cRegRead16(int i2cID, int reg, bool highBitFirst) {
  Wire.beginTransmission(i2cID);   // select device with "beginTransmission()"
  Wire.write(reg & 0x0FF);              // select starting register with "write()"
  Wire.endTransmission();               // end write operation, as we just wanted to select the starting register
  
  Wire.requestFrom(i2cID, 2);      // select number of bytes to get from the device (2 bytes in this case)
  unsigned int msb;
  unsigned int lsb;
  if (highBitFirst) {
    msb = Wire.read();
    lsb = Wire.read();
  }
  else {
    lsb = Wire.read();
    msb = Wire.read();
  }
  return ((msb & 0x0FF) << 8) + (lsb & 0x0FF);
}

int mpu9255::i2cRegRead8(int i2cID, int reg) {
  Wire.beginTransmission(i2cID);   // select device with "beginTransmission()"
  Wire.write(reg & 0x0FF);              // select starting register with "write()"
  Wire.endTransmission();               // end write operation, as we just wanted to select the starting register
  
  Wire.requestFrom(i2cID, 1);      // select number of bytes to get from the device (2 bytes in this case)
  unsigned int data = Wire.read();
  return (data & 0x0FF);
}

int mpu9255::i2cRegWrite8(int i2cID, int reg, int data) {
  Wire.beginTransmission(i2cID);   // select device with "beginTransmission()"
  Wire.write(reg & 0x0FF);
  Wire.write(data & 0x0FF);
  Wire.endTransmission();
}

//sets up bypass to compass i2c, to access compass registers directly over i2c
void mpu9255::bypassOn() {
  if (bypOn == false) {
    unsigned char userControl = i2cRegRead8(IMU_I2C_ID, USER_CTRL);
    userControl &= ~0x20;
    i2cRegWrite8(IMU_I2C_ID, USER_CTRL, userControl);
    delay(50);
    i2cRegWrite8(IMU_I2C_ID, INT_PIN_CFG, 0x82);
    delay(50);
    bypOn = true;
  }
}

void mpu9255::bypassOff() {
  if (bypOn == true) {
    unsigned char userControl = i2cRegRead8(IMU_I2C_ID, USER_CTRL);
    userControl |= 0x20;
    i2cRegWrite8(IMU_I2C_ID, USER_CTRL, userControl);
    delay(50);
    i2cRegWrite8(IMU_I2C_ID, INT_PIN_CFG, 0x80);
    delay(50);
    bypOn = false;
  }
}











//Helper Functions

int mpu9255::signExtend(int data, int sign_extend_index) {
  int sign_extend_index_mask = 0x01 << sign_extend_index;

  /* create mask for bits to the left of the index you want to sign extend from */
  int extendLeft = (~sign_extend_index_mask) - (sign_extend_index_mask - 1);

  if (data & sign_extend_index_mask) {
    data |= extendLeft;
  }
  else {
    data &= (~extendLeft);
  }
  return data;
  
}









//Madgwick Algorithm

void mpu9255::updateQuaternion() {
  //MadgwickUpdate(gX, gY, gZ, aX, aY, aZ);
  MadgwickUpdate(gyro[X_AXIS], gyro[Y_AXIS], gyro[Z_AXIS], accel[X_AXIS], accel[Y_AXIS], accel[Z_AXIS], compass[X_AXIS], compass[Y_AXIS], compass[Z_AXIS]);
}

void mpu9255::MadgwickUpdate(float gx, float gy, float gz, float ax, float ay, float az) {
  if (lastUpdateTime == -1) {
    lastUpdateTime = millis();
    return;
  }

  unsigned long currentTime = millis();
  int sampleFreq = 1000 / (currentTime - lastUpdateTime);
  //Serial.println("sampleFreq: " + String(sampleFreq));
  lastUpdateTime = currentTime;
  
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
  qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
  qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
  qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;   

    // Auxiliary variables to avoid repeated arithmetic
    _2q0 = 2.0f * q0;
    _2q1 = 2.0f * q1;
    _2q2 = 2.0f * q2;
    _2q3 = 2.0f * q3;
    _4q0 = 4.0f * q0;
    _4q1 = 4.0f * q1;
    _4q2 = 4.0f * q2;
    _8q1 = 8.0f * q1;
    _8q2 = 8.0f * q2;
    q0q0 = q0 * q0;
    q1q1 = q1 * q1;
    q2q2 = q2 * q2;
    q3q3 = q3 * q3;

    // Gradient decent algorithm corrective step
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
    recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // Apply feedback step
    qDot1 -= beta * s0;
    qDot2 -= beta * s1;
    qDot3 -= beta * s2;
    qDot4 -= beta * s3;
  }

  // Integrate rate of change of quaternion to yield quaternion
  q0 += qDot1 * (1.0f / sampleFreq);
  q1 += qDot2 * (1.0f / sampleFreq);
  q2 += qDot3 * (1.0f / sampleFreq);
  q3 += qDot4 * (1.0f / sampleFreq);

  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}

void mpu9255::MadgwickUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
  if (lastUpdateTime == -1) {
    lastUpdateTime = millis();
    return;
  }

  unsigned long currentTime = millis();
  int sampleFreq = 1000 / (currentTime - lastUpdateTime);
  //Serial.println("sampleFreq: " + String(sampleFreq));
  lastUpdateTime = currentTime;
  
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float hx, hy;
  float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

  // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
  if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
    Serial.println("Invalid Compass Data in Madgwick Algorithm");
    MadgwickUpdate(gx, gy, gz, ax, ay, az);
    return;
  }

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
  qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
  qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
  qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;  
    
    // Normalise magnetometer measurement
    recipNorm = invSqrt(mx * mx + my * my + mz * mz);
    mx *= recipNorm;
    my *= recipNorm;
    mz *= recipNorm;

    // Auxiliary variables to avoid repeated arithmetic
    _2q0mx = 2.0f * q0 * mx;
    _2q0my = 2.0f * q0 * my;
    _2q0mz = 2.0f * q0 * mz;
    _2q1mx = 2.0f * q1 * mx;
    _2q0 = 2.0f * q0;
    _2q1 = 2.0f * q1;
    _2q2 = 2.0f * q2;
    _2q3 = 2.0f * q3;
    _2q0q2 = 2.0f * q0 * q2;
    _2q2q3 = 2.0f * q2 * q3;
    q0q0 = q0 * q0;
    q0q1 = q0 * q1;
    q0q2 = q0 * q2;
    q0q3 = q0 * q3;
    q1q1 = q1 * q1;
    q1q2 = q1 * q2;
    q1q3 = q1 * q3;
    q2q2 = q2 * q2;
    q2q3 = q2 * q3;
    q3q3 = q3 * q3;

    // Reference direction of Earth's magnetic field
    hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
    hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // Apply feedback step
    qDot1 -= beta * s0;
    qDot2 -= beta * s1;
    qDot3 -= beta * s2;
    qDot4 -= beta * s3;
  }

  // Integrate rate of change of quaternion to yield quaternion
  q0 += qDot1 * (1.0f / sampleFreq);
  q1 += qDot2 * (1.0f / sampleFreq);
  q2 += qDot3 * (1.0f / sampleFreq);
  q3 += qDot4 * (1.0f / sampleFreq);

  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
/*float mpu9255::invSqrt(float x) {
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}*/

//https://pizer.wordpress.com/2008/10/12/fast-inverse-square-root/
float mpu9255::invSqrt(float x){
   uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
   float tmp = *(float*)&i;
   return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
}

