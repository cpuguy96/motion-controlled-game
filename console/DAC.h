
#ifndef DAC_H
#define DAC_H

enum CHORD{NOTEA, NOTEB, NOTES_PER_CHORD};
enum INSTRUMENTS{PIANO, BASS, NUM_INSTRUMENTS};

#define BUS_FREQ 80000000
#define NUMWAVESAMPLES 64
#define DACPRECISION 4096
#define PF2       (*((volatile uint32_t *)0x40025010))

/***************** DAC_Init ****************
	Initializes DAC, timers, and SSI
	Inputs: none
	Outputs: none
*/
void DAC_Init(void);
/***************** Enable_DAC ****************
	Turns on DAC output 
	Inputs: none
	Outputs: none
*/
void Enable_DAC(void);
/***************** Disable_DAC ****************
	Turns off DAC output 
	Inputs: none
	Outputs: none
*/
void Disable_DAC(void);
/***************** DAC_Test ****************
	Test function to see how DAC outputs sine wave
	Inputs: none
	Outputs: none
*/
void DAC_Test(void);

void Set_Note(uint8_t note, uint32_t frequency, uint8_t instrument);
#endif
