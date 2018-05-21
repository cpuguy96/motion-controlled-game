
#ifndef __music_h
#define __music_h

#define INIT_SONG TEST


#define MAXWAVEAMP 2048
#define TRUE 1
#define FALSE 0
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
typedef struct Note_t{
	uint32_t frequency;									// in hertz
	uint8_t length;											// in number of 16th notes; 1 is 16th, 4 is quarter, 8 is half, 16 is whole 
} Note;

typedef struct Song_t{
	uint32_t numNotesA;									// number of NoteA in song
	uint32_t numNotesB;									// number of NoteB in song
	uint32_t BPM;												
	const Note* songNotesA;							// array of NoteA for song
	const Note* songNotesB;							// array of NoteB for song
	uint8_t instrument;									// type of instrument the song uses
} Song;
/***************** Music_Init ****************
	Initializes Systick and starts default song
	Inputs: none
	Outputs: none
*/
void Music_Init(void);
/***************** Song_Init ****************
	Sets and initializes current song 
	Inputs: none
	Outputs: none
*/
void Song_Init(uint8_t songNumber);
/***************** Play_Pause ****************
	Plays/pauses depending on if music is playing or not
	Inputs: none
	Outputs: none
*/
void Play_Pause(void);

#endif
