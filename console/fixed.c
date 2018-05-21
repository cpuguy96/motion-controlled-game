
// filename ******** fixed.c **************
// Cole Thompson cnt584
// Chimezie Iwuanyanwu cci273
// 1/23/18

#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "fixed.h"

static int32_t MinX = 0;
static int32_t MaxX = 0;
static int32_t MinY = 0;
static int32_t MaxY = 0;

/****************ST7735_sDecOut2_MoreRange***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99999.99 to +99999.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD
Parameter LCD display
 */
void ST7735_sDecOut2_MoreRange(int32_t n) {
	if (n >= 0) {
		ST7735_OutChar(' ');
	}
	else {
		ST7735_OutChar('-');
		n *= -1;
	}

	if (n > 9999999 || n < -9999999) {
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('.' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
	}
	else {
		int32_t digit, i;
		for (i = 1000000; i > 0; i /= 10) {
			digit	= n / i;
			n -= digit * i;
			if (i == 10) {
				ST7735_OutChar('.' );
			}
			ST7735_OutChar(0x30 + digit);
		}
		
		/* no loop but less division so faster
		digit	= n / 1000000;
		n -= digit * 1000000;
		ST7735_OutChar(0x30 + digit);
		
		digit	= n / 100000;
		n -= digit * 100000;
		ST7735_OutChar(0x30 + digit );
		
		digit	= n / 10000;
		n -= digit * 10000;
		ST7735_OutChar(0x30 + digit );
		
		digit	= n / 1000;
		n -= digit * 1000;
		ST7735_OutChar(0x30 + digit );
		
		digit = n / 100;
		n -= digit * 100;
		ST7735_OutChar(0x30 + digit );

		ST7735_OutChar('.' );

		digit = n / 10;
		n -= digit * 10;
		ST7735_OutChar(0x30 + digit );

		digit = n;
		ST7735_OutChar(0x30 + digit );
		*/
	}
}

/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"
 -8100    "-81.00"
  -102    " -1.02"
    31    "  0.31"
-12345    "-**.**"
 */
void ST7735_sDecOut2(int32_t n) {
	if (n >= 0) {
		ST7735_OutChar(' ' );
	}
	else {
		ST7735_OutChar('-' );
		n *= -1;
	}

	if (n > 9999 || n < -9999) {
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('.' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
	}
	else {
		int32_t digit = n / 1000;
		n -= digit * 1000;
		ST7735_OutChar(0x30 + digit );
		digit = n / 100;
		n -= digit * 100;
		ST7735_OutChar(0x30 + digit );

		ST7735_OutChar('.' );

		digit = n / 10;
		n -= digit * 10;
		ST7735_OutChar(0x30 + digit );

		digit = n;
		ST7735_OutChar(0x30 + digit );
	}
}

/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64.
 The full-scale range is from 0 to 999.99.
 If the integer part is larger than 63999, it signifies an error.
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/
void ST7735_uBinOut6(uint32_t n) {
	uint32_t intPart = n >> 6;
	//+32 for rounding
	uint32_t decPart = ((n * 100 + 32) / 64) % 100;
	uint32_t digitT;

	if (intPart > 999 || intPart < 0) {
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
		ST7735_OutChar('.' );
		ST7735_OutChar('*' );
		ST7735_OutChar('*' );
	}
	else {
		uint32_t digitH = intPart / 100;
		intPart -= digitH * 100;
		if (digitH == 0) {
			ST7735_OutChar(' ' );
		}
		else {
			ST7735_OutChar(0x30 + digitH );
		}

		digitT = intPart / 10;
		intPart -= digitT * 10;
		if (digitT == 0 && digitH == 0) {
			ST7735_OutChar(' ' );
		}
		else {
			ST7735_OutChar(0x30 + digitT );
		}

		ST7735_OutChar(0x30 + intPart );

		ST7735_OutChar('.' );

		digitT = decPart / 10;
		decPart -= digitT * 10;
		ST7735_OutChar(0x30 + digitT );
		ST7735_OutChar(0x30 + decPart );

	}
}

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot -termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_FillRect(0, 32, 128, 128, ST7735_WHITE);
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	MinX = minX;
	MaxX = maxX;
	MinY = minY;
	MaxY = maxY;
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]) {
	//TODO check bounds for points
	int n;
	for (n = 0; n < num; n++) {
		// i goes from 0 to 127
		// x=MaxX maps to i=0
		// x=MaxX maps to i=127
		int32_t i = (127*(bufX[n]-MinX))/(MaxX-MinX);
		// y=MaxY maps to j=32
		// y=MinY maps to j=159
		int32_t j = 32+(127*(MaxY-bufY[n]))/(MaxY-MinY);

		ST7735_DrawPixel(i,   j,   ST7735_BLUE);
		ST7735_DrawPixel(i+1, j,   ST7735_BLUE);
		ST7735_DrawPixel(i,   j+1, ST7735_BLUE);
		ST7735_DrawPixel(i+1, j+1, ST7735_BLUE);
	}

}
