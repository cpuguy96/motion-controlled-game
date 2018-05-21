//MPU9255 10-DOF IMU Class
//For EE445L Final Project at UT Austin ECE

#ifndef MPU9255
#define MPU9255

#include <Arduino.h>
#include <Ticker.h>  //Ticker Library

#include "vec3.h"

//REGISTERS
//H=high, L=low, high and low bytes of 16 bit data

//MPU9255 General
#define IMU_I2C_ID          0x68
#define MPU9255_WHO_AM_I    0x75
#define MPU9255_ID          0x73
#define USER_CTRL       0x6A
#define SMPRT_DIV       0x19
#define INT_PIN_CFG     0x37
#define PWR_MGMT_1      0x6b
#define PWR_MGMT_2      0x6c
#define FIFO_EN         0x23

//Accelerometer
#define ACCEL_CONFIG        0x1c
#define ACCEL_LPF           0x1d
#define ACCEL_XOUT_H        0x3b
#define ACCEL_XOUT_L        0x3c
#define ACCEL_YOUT_H        0x3d
#define ACCEL_YOUT_L        0x3e
#define ACCEL_ZOUT_H        0x3f
#define ACCEL_ZOUT_L        0x40

//Gyroscope
#define GYRO_CONFIG         0x1b
#define GYRO_LPF            0x1a
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48

//Compass (accessed from MPU SLV regs)
#define I2C_MST_CTRL    0x24
#define I2C_MST_DELAY_CTRL 0x67
#define I2C_SLV4_CTRL   0x34    //speed control for all slaves I2C_SLV4_CTRL[3:0]

#define I2C_SLV0_ADDR   0x25    //slv0 assigned to compass data regs
#define I2C_SLV0_REG    0x26
#define I2C_SLV0_CTRL   0x27
#define CMPS_XOUT_L   0x4A 
#define CMPS_XOUT_H   0x4B 
#define CMPS_YOUT_L   0x4C
#define CMPS_YOUT_H   0x4D 
#define CMPS_ZOUT_L   0x4E
#define CMPS_ZOUT_H   0x4F

#define I2C_SLV1_ADDR   0x28  //slv1 assigned to compass CNTL reg
#define I2C_SLV1_REG    0x29
#define I2C_SLV1_CTRL   0x2A
#define I2C_SLV1_DO     0x64

//Compass (accessed from bypass mode, directly to compass)
#define AK8963_I2C_ID   0x0C
#define AK8963_CNTL     0x0A
#define AK8963_X_L      0x03
#define AK8963_X_H      0x04
#define AK8963_Y_L      0x05
#define AK8963_Y_H      0x06
#define AK8963_Z_L      0x07
#define AK8963_Z_H      0x08
#define AK8963_ASAX     0x10
#define AK8963_ASAY     0x11
#define AK8963_ASAZ     0x12


#define TIMER_UPDATE_MS 20

#define SWIPE_TOLERANGE_ANGLE_D 55    // acceleration angle degrees
#define SWIPE_TOLERANGE_MAG .7       // acceleration magnitude (gs)

#define SWIPE_TOLERANGE_ANGLE_R ((SWIPE_TOLERANGE_ANGLE_D * PI) / 180)    // acceleration angle radians


/* directions relative to the user 
 * the IMU will be oriented in the controller so that the orientation during calibration maps to the following directions:
 * F : -Y     B : +Y
 * U : +Z     D : -Z
 * L : +X     R : -X
 */
enum Directions{L, R, B, F, U, D, N, S, E, W}; 

class mpu9255 {

public:
	mpu9255();

  /* Sensor Data */

  //use most recent values in double gyro[3], accel[3], compass[3]
  void updateQuaternion();

  //update all sensor axes, updates gyro[3], accel[3], compass[3]
  void update9Axes();

  //update 3 axes of one sensor, updates gyro[3], accel[3], compass[3], using 6 byte i2c burst read
  void updateGyro();
  void updateAccel();
  void updateCompass();

  //update and return a single axis of a sensor, updates gyro[3], accel[3], compass[3], using 2 byte i2c burst read
  double updateGyroAxis(int axis);
  double updateAccelAxis(int axis);
  double updateCompassAxis(int axis);

  //get most recently updated values from a sensor, reads from gyro[3], accel[3], compass[3]
  double getGyroAxis(int axis);
  double getAccelAxis(int axis);
  double getCompassAxis(int axis);

  //get vectors for most recently updated values from a sensor
  vec3 getGyroVec();
  vec3 getAccelVec();
  vec3 getCompassVec();
  

  /* Math */

  //Vector and rotation stuff
  void deviceToWorldSpace(vec3& v);         //rotate a vector in device space (relative to the chip) into world space (North, east, south, west)
  void worldToDeviceSpace(vec3& v); 
  void worldToUserSpace(vec3& v);           //rotate a vector in world space (North, east, south, west) to user space (initial position)
  void userToWorldSpace(vec3& v);
  int getVectorUserDirection(vec3 v);       //return userdirection enum for the axis that a vector is closest to, for a vector in user space
  int getVectorWorldDirection(vec3 v);      //return worlddirection enum for the axis that a vector is closest to, for a vector in world space

  //acceleration in user space (for detecting gestures)
  vec3 getAccelDirectionUser();    //return accel vector, rotated into user space with reference to initial position, minus gravity
  double getAccelMag();

  //some methods of checking for gesures
  bool checkForGesture();                 //checks if magnitude is greater than a constant value
  int checkForGestureAxis(int axis);      //returns +1, 0, -1, takes vector axes enum
  
  //things to return for the game
  vec3 checkForGestureAxis();             //returns +1, 0, -1 for each axis
  double getRotationAroundAxis(int axis);   //returns degrees. for the controls of the game, direction of look for character


  

  //set the offsets for each sensor
  void calibrate();
  void setInitialOrientation();

  //methods to convert data to strings
  String dataToString();    //make sure to update9Axes and updateQuaternion first
  String quaternionVisualizerString();
  String packetString();
  String offsetsToString();
  char dirToChar(int dir);    //Directions enum
  String worldOrientationTestString();
   
  
private:


  //most recent data for each sensor, updated from the update functions
  double gyro[3] = {0};
  double accel[3] = {0};
  double compass[3] = {0};

  //calibration data
  int numDataPoints = 100;
  double gyroOffsets[3] = {0};
  double accelOffsets[3] = {0};
  double compassOffsets[3] = {0};
  double compassSensitivity[3] = {0};
  volatile float q0i = 1.0f, q1i = 0.0f, q2i = 0.0f, q3i = 0.0f;  //initial orientation of the device

  //convert raw data to SI units, based on sensitivity and calibration offsets
  double convertRawAccel(int data, int axis);
  double convertRawGyro(int data, int axis);
  double convertRawCompass(int data, int axis);

  //i2c bypass to the compass sensor
  bool bypOn = false;
  void bypassOn();
  void bypassOff();

  //i2c functions for reading and writing from registers
  void i2cRegRead(int i2cID, int reg, int numBytes, int* data);
  int i2cRegRead16(int i2cID, int reg, bool highBitFirst);
  int i2cRegRead8(int i2cID, int reg);
  int i2cRegWrite8(int i2cID, int reg, int data);

  int signExtend(int data, int sign_extend_index);



  //Madgwick Algorithm
  unsigned long lastUpdateTime = -1;
  volatile float beta = 0.1f;                // 2 * proportional gain (Kp)
  volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;  // quaternion of sensor frame relative to auxiliary frame

  void MadgwickUpdate(float gx, float gy, float gz, float ax, float ay, float az);
  void MadgwickUpdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
  float invSqrt(float x);

};


#endif
