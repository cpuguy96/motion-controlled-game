
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "Music.h"
#include "SysTickInts.h"
#include "DAC.h"
#include "Music_Constants.h"

long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

static uint8_t CurrentSong;					
static uint8_t IsPlaying;						// checks if song is playing

static uint8_t CountA;					// counts the number of 1/16th NoteA notes played
static uint8_t CountB;					// counts the number of 1/16th NoteB notes played 

static uint32_t NoteIndexA;			// index of which NoteA is playing
static uint32_t NoteIndexB;			// index of which NoteB is playing

static uint8_t BeatCount;				// used for toggling PF3 every beat


/***************** SysTickTask ****************
	Checks every 16th note if a new note needs to be played
	Inputs: none
	Outputs: none
*/
void SysTickTask(void) {
	CountA++;
	CountB++;
	
	if (IsPlaying) {
		long sr = StartCritical();
		Song s = SONGS[CurrentSong];
		
		if (CountA == s.songNotesA[NoteIndexA].length) {
			NoteIndexA++;
			if (NoteIndexA == s.numNotesA) {
				Song_Init(CurrentSong); //loop at end of song
			}
			else {
				Set_Note(NOTEA, s.songNotesA[NoteIndexA].frequency, PIANO);
			}
			CountA = 0;
		}
		if (CountB == s.songNotesB[NoteIndexB].length) {
			NoteIndexB++;
			if (NoteIndexB == s.numNotesB) {
				Song_Init(CurrentSong);
			}
			else {
				Set_Note(NOTEB, s.songNotesB[NoteIndexB].frequency, PIANO);
			}
			CountB = 0;
		}
		EndCritical(sr);
	}
	
	BeatCount = (BeatCount + 1) % 16;
}
/***************** Play ****************
	Plays song
	Inputs: none
	Outputs: none
*/
void Play(void){
	IsPlaying = TRUE;
	Play_SysTick();
	Enable_DAC();
}
/***************** Pause ****************
	Pauses song
	Inputs: none
	Outputs: none
*/
void Pause(void){
	IsPlaying = FALSE;
	Pause_SysTick();
	Disable_DAC();
}

/***************** Music_Init ****************
	Initializes Systick and starts default song
	Inputs: none
	Outputs: none
*/
void Music_Init(void){
	CurrentSong = INIT_SONG;
	SysTick_Init(&SysTickTask, 80000000);
	Song_Init(INIT_SONG);
}
/***************** Play_Pause ****************
	Plays/pauses depending on if music is playing or not
	Inputs: none
	Outputs: none
*/
void Play_Pause(void){
	if(IsPlaying){
		Pause();
	}
	else{
		Play();
	}
}

/***************** Song_Init ****************
	Sets and initializes current song 
	Inputs: none
	Outputs: none
*/
void Song_Init(uint8_t songNumber){
	long sr = StartCritical();
	CurrentSong = songNumber;
	
	//set systick interrupt every 16th note at the songs bpm
	uint32_t song16thNoteClockCycles = 300000000 / (SONGS[CurrentSong].BPM); //(BUS_FREQ * 60) / (SONGS[songNumber].BPM * 16);
	SysTick_Reset_Per(song16thNoteClockCycles);
	Song s = SONGS[CurrentSong];
	
	NoteIndexA = 0;
	CountA = 0;
	Set_Note(NOTEA, s.songNotesA[NoteIndexA].frequency, PIANO);
	
	NoteIndexB = 0;
	CountB = 0;
	Set_Note(NOTEB, s.songNotesB[NoteIndexB].frequency, PIANO);
	
	IsPlaying = TRUE;
	BeatCount = 0;
	EndCritical(sr);
}


