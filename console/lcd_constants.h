#ifndef __lcd_constants_h
#define __lcd_constants_h

/*********** Constant Array Values *************/

// Holds x1, y1, and x2, y2 coordinate pairs to draw digits
// coordinates are just offsets that to be added to a starting point
// width variable is multiplied to
/*  0-> 
1|  *-* 2|
 v  | |  v
3->	*-*	
4|	| | 5|
 v	*-*	 v
		6->
*/
const uint8_t DIGIT_VECTORS [10][7] = {
	{1, 1, 1, 0, 1, 1, 1},// zero
	{0, 0, 1, 0, 0, 1, 0},// one
	{1, 0, 1, 1, 1, 0, 1},// two
	{1, 0, 1, 1, 0, 1, 1},// three
	{0, 1, 1, 1, 0, 1, 0},// four
	{1, 1, 0, 1, 0, 1, 1},// five
	{1, 1, 0, 1, 1, 1, 1},// six
	{1, 0, 1, 0, 0, 1, 0},// seven
	{1, 1, 1, 1, 1, 1, 1},// eight
	{1, 1, 1, 1, 0, 1, 1},// nine
};


/*********** Images **********************/


#endif
