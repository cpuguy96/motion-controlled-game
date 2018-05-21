#ifndef VEC3_H
#define VEC3_H

#include <Arduino.h>

/* Simple class, used for vector operations
 * rotate vector by a quaternion
 * get the axis that the vector is closest to
 * 
 */

enum Axes {X_AXIS, Y_AXIS, Z_AXIS};


class vec3 {

public:
  vec3(double x, double y, double z);
  vec3();

  double getAxis(int axis);
  void setAxis(int axis, double data);
  
  int getClosestAxis();   //return the axis that this vector is closest to. returns axis enum + 1, *-1 if pointing in the negative direction of the axis
  double getAngleToAxis(int axis);   //angle between magnitude and desired axis in rads
  void rotateByQuaternion(double q0, double q1, double q2, double q3);
  double mag();
  
  String toString();
  String closestAxisToString();

  /* Operator Overloading */
  vec3& operator=(vec3& other);
  vec3& operator+=(vec3& other);
  vec3& operator-=(vec3& other);
  vec3& operator*=(vec3& other);
  vec3& operator*=(double other);
  vec3& operator+(vec3& other);
  vec3& operator-(vec3& other);
  vec3& operator*(vec3& other);
  vec3& operator*(double other);
  bool operator==(vec3& rhs);
  bool operator!=(vec3& rhs);
  
  static vec3 cross(vec3 a, vec3 b);
  static double dot(vec3 a, vec3 b);
  static void invertQuat(double* q0, double* q1, double* q2, double* q3);
  static double absD(double x);

private:

  double vector[3] = {0};
  
};

#endif
