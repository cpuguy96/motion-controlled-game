
#ifndef __music_constants_h
#define __music_constants_h


#define B2 987

//divide by 2 to shift down an octave


#define B 494
#define Bb 466
#define A 440
#define Ab 415
#define G 392
#define Gb 370
#define F 349
#define E 330
#define Eb 311
#define D 294
#define Db 277
#define C 262


#define _16TH 1
#define QUART 4
#define HALF 8
#define WHOLE 16

#define NUM_NOTES_FUR_ELISE_MELODY 40
#define NUM_NOTES_FUR_ELISE_HARMONY 25

enum SONG_NUM{TEST, FUR_ELISE, NUM_SONGS};

const Note TEST_NOTESA[1] = {
	{A, WHOLE},
};
const Note TEST_NOTESB[1] = {
	{A, WHOLE},
};


const Note FUR_ELISE_MELODY[NUM_NOTES_FUR_ELISE_MELODY] = {
	{E * 2, QUART},
	{Eb * 2, QUART},
	{E * 2, QUART},
	{Eb * 2, QUART},
	{E * 2, QUART},
	{B, QUART},
	{D * 2, QUART},
	{C * 2, QUART},
	
	{A, QUART},
	{1, QUART * 2},
	{C, QUART},
	{E, QUART},
	{A, QUART},
	
	{B, QUART},
	{1, QUART * 2},
	{E, QUART},
	{Ab, QUART},
	{B, QUART},
	
	{C*2, QUART},
	{1, QUART * 2},
	{E, QUART},
	{E * 2, QUART},
	{Eb * 2, QUART},
	
	{E * 2, QUART},
	{Eb * 2, QUART},
	{B, QUART},
	{D * 2, QUART},
	{C * 2, QUART},
	{A, QUART},
	
	{1, QUART * 2},
	{C, QUART},
	{E, QUART},
	{A, QUART},
	{B, QUART},
	{1, QUART * 2},
	
	{E * 2, QUART},
	{C * 2, QUART},
	{B, QUART},
	{A, QUART},
	{1, QUART * 2}
};


const Note FUR_ELISE_HARMONY[NUM_NOTES_FUR_ELISE_HARMONY] = {
	{1, QUART * 8},
	
	{A / 4, QUART},
	{E / 2, QUART},
	{A /2, QUART},
	{1, QUART * 3},
	
	{E / 4, QUART},
	{E / 2, QUART},
	{Ab / 2, QUART},
	{1, QUART * 3},
	
	{1, QUART},
	{Eb * 2, QUART},
	{B, QUART},
	{1, QUART * 3},
		
	{1, QUART * 5},
	{A / 4, QUART},
	
	{E / 2, QUART},
	{A / 2, QUART},
	{1, QUART * 3},
	{E / 4, QUART},
	{E / 2, QUART},
	{Ab / 2, QUART},
	
	{1, QUART * 3},
	{A / 4, QUART},
	{E / 2, QUART},
	{A / 2, QUART}
};

const Song SONGS[10] = {
 {1, 1, 60, TEST_NOTESA, TEST_NOTESB, PIANO}, 
 {NUM_NOTES_FUR_ELISE_MELODY, NUM_NOTES_FUR_ELISE_HARMONY, 120/2, FUR_ELISE_MELODY, FUR_ELISE_HARMONY, PIANO} 
};

#endif
