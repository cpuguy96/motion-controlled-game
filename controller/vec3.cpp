#include "vec3.h"


vec3::vec3(double x, double y, double z) {
  vector[X_AXIS] = x;
  vector[Y_AXIS] = y;
  vector[Z_AXIS] = z;
}

vec3::vec3() {
  vector[X_AXIS] = 0;
  vector[Y_AXIS] = 0;
  vector[Z_AXIS] = 0;
}

double vec3::getAxis(int axis) {
  return vector[axis];
}

void vec3::setAxis(int axis, double data) {
  vector[axis] = data;
}
double vec3::mag() {
  return sqrt((vector[X_AXIS] * vector[X_AXIS]) + (vector[Y_AXIS] * vector[Y_AXIS]) + (vector[Z_AXIS] * vector[Z_AXIS])  );
}

double vec3::getAngleToAxis(int axis) {
  return acos(absD(getAxis(axis)) / mag());
}

int vec3::getClosestAxis() {
  double currentMag = 0;
  double maxMag = 0;
  int maxAxis = 0;
  for (int i = 0; i < 3; i++) {
    currentMag = absD(getAxis(i));
    if (currentMag > maxMag) {
      maxAxis = i;
      maxMag = currentMag;
    }
  }

  bool pos = getAxis(maxAxis) > 0;
  maxAxis += 1;   //cant have -0
  return pos?maxAxis:(maxAxis * -1);
}

String vec3::closestAxisToString() {
  int closestAxisRet = getClosestAxis();
  bool pos = closestAxisRet > 0;
  closestAxisRet = abs(closestAxisRet) - 1;   //getClosestAxis returns (axis+1) so that it can be negative, otherwise +0 -0 would be a problem

  String s = "";
  if (closestAxisRet == X_AXIS) {
    s = pos?"+X":"-X";
  }
  else if (closestAxisRet == Y_AXIS) {
    s = pos?"+Y":"-Y";
  }
  else if (closestAxisRet == Z_AXIS) {
    s = pos?"+Z":"-Z";
  }
  return s;
}


//this didnt work
/*void vec3::rotateByQuaternion(double q0, double q1, double q2, double q3) {
  vec3 quat = vec3(q1, q2, q3);
  vec3 uv = cross(quat, *this);
  vec3 uuv = cross(quat, uv);
  *this += ((uv * q0) + uuv) * 2.0;
}*/

void vec3::rotateByQuaternion(double q0, double q1, double q2, double q3) {
  /* Rotation Matrix Q
   * q0^2+q1^2−q2^2−q3^2    2q1q2-2q0q3               2q1q3+2q0q2
   * 2q1q2+2q0q3            q0^2−q1^2+q2^2−q3^2     2q2q3-2q0q1
   * 2q1q3-2q0q2             2q2q3+2q0q1              q0^2−q1^2−q2^2+q3^2
   * 
   * Q*v = rotated v
   */
  vec3 r0 = vec3((q0*q0 + q1*q1 - q2*q2 - q3*q3), (2*q1*q2 - 2*q0*q3), (2*q1*q3 + 2*q0*q2));
  vec3 r1 = vec3((2*q1*q2 + 2*q0*q3), (q0*q0 - q1*q1 + q2*q2 - q3*q3), (2*q2*q3 - 2*q0*q1));
  vec3 r2 = vec3((2*q1*q3 - 2*q0*q2), (2*q2*q3 + 2*q0*q1), (q0*q0 - q1*q1 - q2*q2 + q3*q3));

  vec3 thisVec = *this;
  
  vector[0] = dot(r0, thisVec);
  vector[1] = dot(r1, thisVec);
  vector[2] = dot(r2, thisVec);
}

vec3 vec3::cross(vec3 a, vec3 b) {
  //(u2v3 - u3v2)i + (u3v1 - u1v3)j + (u1v2 - u2v1)k
  double x = a.getAxis(Y_AXIS) * b.getAxis(Z_AXIS) - a.getAxis(Z_AXIS) * b.getAxis(Y_AXIS);
  double y = a.getAxis(Z_AXIS) * b.getAxis(X_AXIS) - a.getAxis(X_AXIS) * b.getAxis(Z_AXIS);
  double z = a.getAxis(X_AXIS) * b.getAxis(Y_AXIS) - a.getAxis(Y_AXIS) * b.getAxis(X_AXIS);
  return vec3(x, y, z);
}

double vec3::dot(vec3 a, vec3 b) {
  double x = a.getAxis(X_AXIS) * b.getAxis(X_AXIS);
  double y = a.getAxis(Y_AXIS) * b.getAxis(Y_AXIS);
  double z = a.getAxis(Z_AXIS) * b.getAxis(Z_AXIS);
  return x + y + z;
}

//this didnt work
/*void vec3::invertQuat(double* q0, double* q1, double* q2, double* q3) {
  double dotP = (*q0) * (*q0) + (*q1) * (*q1) + (*q2) * (*q2) + (*q3) * (*q3);
  *q0 = (*q0) / dotP;
  *q1 = -1 * (*q1) / dotP;
  *q2 = -1 * (*q2) / dotP;
  *q3 = -1 * (*q3) / dotP;
}*/

void vec3::invertQuat(double* q0, double* q1, double* q2, double* q3) {
  *q1 = -1 * (*q1);
  *q2 = -1 * (*q2);
  *q3 = -1 * (*q3);
}

double vec3::absD(double x) {
  return (x >= 0)?x:(x * -1);
}

String vec3::toString() {
  return String("x:" + String(getAxis(X_AXIS)) + " y:" + String(getAxis(Y_AXIS)) + " z:" + String(getAxis(Z_AXIS)));
}







/* Operator Overloading */

vec3& vec3::operator=(vec3& other) {
  for (int i = 0; i < 3; i++) {
    vector[i] = other.getAxis(i);
  }
  return *this;
}

vec3& vec3::operator+=(vec3& other) {
  for (int i = 0; i < 3; i++) {
    vector[i] += other.getAxis(i);
  }
  return *this;
}

vec3& vec3::operator-=(vec3& other) {
  for (int i = 0; i < 3; i++) {
    vector[i] -= other.getAxis(i);
  }
  return *this;
}

vec3& vec3::operator*=(vec3& other) {
  for (int i = 0; i < 3; i++) {
    vector[i] *= other.getAxis(i);
  }
  return *this;
}

vec3& vec3::operator*=(double other) {
  for (int i = 0; i < 3; i++) {
    vector[i] *= other;
  }
  return *this;
}

vec3& vec3::operator+(vec3& other) {
  vec3 result = vec3(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    result.setAxis(i, (vector[i] + other.getAxis(i)));
  }
  return *this;
}

vec3& vec3::operator-(vec3& other) {
  vec3 result = vec3(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    result.setAxis(i, (vector[i] - other.getAxis(i)));
  }
  return *this;
}

vec3& vec3::operator*(vec3& other) {
  vec3 result = vec3(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    result.setAxis(i, (vector[i] * other.getAxis(i)));
  }
  return *this;
}

vec3& vec3::operator*(double other) {
  vec3 result = vec3(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    result.setAxis(i, (vector[i] * other));
  }
  return *this;
}

bool vec3::operator==(vec3& rhs) {
  bool eq = true;
  for (int i = 0; i < 3; i++) {
    eq = eq && (vector[i] == rhs.getAxis(i));
  }
  return eq;
}

bool vec3::operator!=(vec3& rhs) {
  bool eq = true;
  for (int i = 0; i < 3; i++) {
    eq = eq && (vector[i] == rhs.getAxis(i));
  }
  return !eq;
}


