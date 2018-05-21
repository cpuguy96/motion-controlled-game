#include <stdint.h>
#include "trig.h"


const uint16_t SINE[91] = {0,
0,1,3,5,8,11,15,19,
24,30,36,43,51,59,67,76,
85,95,106,117,128,140,153,165,
179,192,206,220,235,250,265,281,
297,313,329,345,362,379,396,413,
430,448,465,483,500,517,535,552,570,587,604,621,
638,655,671,687,703,719,735,750,
765,780,794,808,821,835,847,860,
872,883,894,905,915,924,933,941,
949,957,964,970,976,981,985,989,
992,995,997,999,1000,1000};


/***************** Sine ****************
	Calculates the sine of an angle in degrees; 0.001 resolution
	Inputs: angle - angle in degrees
	Outputs: sine of angle
*/
int16_t Sine(uint16_t angle){
	if(angle <= 90){
		return SINE[angle];
	}
	else if(angle <= 180){
		return SINE[90 - (angle - 90)];
	}
	else if(angle <= 270){
		return -1 * SINE[angle - 180];
	}
	else if(angle <= 360){
		return -1 * SINE[180 - (angle - 180)];
	}
	else{
			return Sine(angle % 360);
	}
}

/***************** Cosine ****************
	Calculates the cosine of an angle in degrees; 0.001 resolution
	Inputs: angle - angle in degrees
	Outputs: sine of angle
*/
int16_t Cosine(uint16_t angle){
	return Sine(angle + 90);
}
