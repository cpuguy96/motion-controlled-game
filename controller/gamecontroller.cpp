#include "gamecontroller.h"

gamecontroller::gamecontroller(int controller_id) {
	id = controller_id;
	imu = new mpu9255();
	js = new joystick(X_EN_PIN, Y_EN_PIN, ADC_PIN, B_JS_PIN);
	b_js = js->getButton();
	b_primary = new button(B_PRIMARY_PIN);
	b_3 = new button(B_3_PIN);
	//LED driver
}

String gamecontroller::toString() {
	String s = "IMU\n";
	s += getImu()->dataToString();
	s += "\nJOYSTICK\n";
  s+= getJs()->toString();
	return s;
}

//{D:id:q0,q1,q2,q3:x_gesture,y_gesture,z_gesture:y_rot:x_js,y_js:b_primary,b_js,b_3}
String gamecontroller::packetStringData() {
  getJs()->updateXY();
  
	String s = String("{D:" + String(getId()) + ":" + getImu()->packetString() + ":" + getJs()->packetString() + ":");
	s += String(getB_primary()->packetString() + "," + getB_js()->packetString() + "," + getB_3()->packetString() + "}");
	return s;
}

//{M:id:message}
String gamecontroller::packetStringMssg(String mssg) {
	return String("{M:" + String(getId()) + ":" + mssg + "}");
}

void gamecontroller::timerUpdateISR() {
  getImu()->update9Axes();        //update imu data
  getImu()->updateQuaternion();   //update quaternion
}

int gamecontroller::getId() {
	return id;
}

mpu9255 * gamecontroller::getImu() {
	return imu;
}

joystick * gamecontroller::getJs() {
	return js;
}

button * gamecontroller::getB_primary() {
	return b_primary;
}

button * gamecontroller::getB_js() {
	return b_js;
}

button * gamecontroller::getB_3() {
	return b_3;
}


