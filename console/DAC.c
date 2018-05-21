
#include <stdint.h>
#include <string.h>
#include "..//inc//tm4c123gh6pm.h"
#include "Timer0A.h"
#include "Timer1.h"
#include "DAC.h"
#include "dac_constants.h"

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

/* Private Variables */

static uint8_t DACEnabled;		

static uint32_t CurrentInstrumentA;
static uint32_t CurrentInstrumentB;

static uint32_t NoteAFrequency;
static uint32_t NoteBFrequency;

static uint8_t WaveIndexA;	// wave index for NoteA
static uint8_t WaveIndexB;	// wave index for NoteB
	
static uint32_t NoteACurrent;	// amplitude of NoteA
static uint32_t NoteBCurrent; // amplitude of NoteB

/* Modular function to compute desired dac amplitude
 * simple right now, but makes it easy to add the envelope button */
/***************** CombineNotes ****************
	Combines amplitude of two different notes
	Inputs: none
	Outputs: sum of NoteA and NoteB
*/
uint32_t CombineNotes(void) {
	return NoteACurrent + NoteBCurrent;
}

//********SSI3_Out*****************
// Send data to SSI3
// inputs:  voltage output (0 to 4095)
// Outputs: none
void SSI3Out(uint16_t code){   
	GPIO_PORTD_DATA_R &= ~0x02;        // PA3 FSS low

  while((SSI3_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI3_DR_R = code; 
	GPIO_PORTD_DATA_R |= 0x02;         // PA3 FSS high

}
/***************** DACOut ****************
	Combines amplitude of two different notes
	Inputs: DAC output value
	Outputs: none
*/
void DACOut(uint16_t value) {
	if (DACEnabled == 1) {
		if (value >= DACPRECISION) {
			value = DACPRECISION - 1;
		}
		SSI3Out(value);		
	}
}
/***************** Timer0Task ****************
	Outputs NoteA amplitude to DAC
	Inputs: none
	Outputs: none
*/
void Timer0Task(void) {
	//set current dac values based on note A, output dac
	PF2 ^= 0x04;
	long sr = StartCritical();
	switch(CurrentInstrumentA){
		case PIANO:
			NoteACurrent = PIANO_WAVE[WaveIndexA];
			break;
		case BASS:
			NoteACurrent = BASS_WAVE[WaveIndexA];
			break;
	}
	WaveIndexA = (WaveIndexA + 1) % NUMWAVESAMPLES;
	DACOut(CombineNotes());
	EndCritical(sr);
	PF2 ^= 0x04;
	PF2 ^= 0x04;
}
/***************** Timer1Task ****************
	Outputs NoteB amplitude to DAC
	Inputs: none
	Outputs: none
*/
void Timer1Task(void) {
	//set current dac values based on note B, output dac
	long sr = StartCritical();
	switch(CurrentInstrumentB){
		case PIANO:
			NoteBCurrent = PIANO_WAVE[WaveIndexB];
			break;
		case BASS:
			NoteBCurrent = BASS_WAVE[WaveIndexB];
			break;
	}
	WaveIndexB = (WaveIndexB + 1) % NUMWAVESAMPLES;
	DACOut(CombineNotes());
	EndCritical(sr);
}

//********SSI3Init*****************
// Initialize SSI3
// inputs:  initial voltage output (0 to 4095)
// assumes: system clock rate less than 20 MHz
void SSI3Init(uint16_t data){
  SYSCTL_RCGCSSI_R |= 0x08;       // activate SSI3
  SYSCTL_RCGCGPIO_R |= 0x08;      // activate port D
  while((SYSCTL_PRGPIO_R&0x01) == 0){};// ready?
  GPIO_PORTD_AFSEL_R |= 0x0B;     // enable alt funct on PD0,1,3
  GPIO_PORTD_DEN_R |= 0x0B;       // configure PD0,1,3 as SSI
   GPIO_PORTD_DATA_R |= 0x02;      // PD1 FSS high

	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00001011;
  GPIO_PORTD_AMSEL_R = 0;         // disable analog functionality on PD
  SSI3_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI3_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI3_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
	SSI3_CR0_R |= 0x80;
  SSI3_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI3_DR_R = data;               		// load '0' into transmit FIFO
  SSI3_CR1_R |= 0x00000002;       // enable SSI
}

/***************** DAC_Init ****************
	Initializes DAC, timers, and SSI
	Inputs: none
	Outputs: none
*/
void DAC_Init(void) {
	//init timers
	Timer0A_Init(&Timer0Task, 80000000);
	Timer1_Init(&Timer1Task, 80000000);
	
	WaveIndexA = 0;
	WaveIndexB = 0;
	NoteACurrent = 0;
	NoteBCurrent = 0;
	NoteAFrequency = 0;
	NoteBFrequency = 0;
	CurrentInstrumentA = PIANO;
	CurrentInstrumentB = PIANO;
	Enable_DAC();
	//init ssi to dac
	SSI3Init(0x1000);
}
/***************** Set_Note ****************
	Sets NoteA value and resets Timer0 frequency
	Inputs: noteA - note to set NoteA to
	Outputs: none
*/
void Set_Note(uint8_t note, uint32_t frequency, uint8_t instrument) {
	switch(note){
	case NOTEA:
		NoteAFrequency = frequency;
		CurrentInstrumentA = instrument;
		if (NoteAFrequency > 1) {
			Timer0A_Reset_Per(BUS_FREQ / (NoteAFrequency * NUMWAVESAMPLES));
			Play_Timer0();
		}
		else {
			Pause_Timer0();
			NoteACurrent = 0;
		}
		break;
	case NOTEB:
		NoteBFrequency = frequency;
		CurrentInstrumentB = instrument;
		if (NoteBFrequency > 1) {
			Timer1_Reset_Per(BUS_FREQ / (NoteBFrequency * NUMWAVESAMPLES));
			Play_Timer1();
		}
		else {
			Pause_Timer1();
			NoteBCurrent = 0;
		}
		break;
	}
}

/***************** Enable_DAC ****************
	Turns on DAC output 
	Inputs: none
	Outputs: none
*/
void Enable_DAC(void) {
	DACEnabled = 1;
}
/***************** Disable_DAC ****************
	Turns off DAC output 
	Inputs: none
	Outputs: none
*/
void Disable_DAC(void) {
	DACEnabled = 0;
}

