/*
 * game.c
 *
 * Functionality related to the game state and features.
 *
 * Author: Jarrod Bennett, Cody Burnett
 */ 

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "ledmatrix.h"
#include "terminalio.h"
#include <stdbool.h>
#include "timer1.h"

//tracks 
static const uint8_t track0[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x80, 0x00, 0x00, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

static const uint8_t track1[TRACK_LENGTH] = {0x10, 
	0x10, 0x10, 0x10, 0x00, 0x01, 0x02, 0x01,
	0x00, 0x00, 0x01, 0x00, 0x04, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x01, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x01, 0x80, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x04, 0x04, 0x80, 0x04, 0x02,
	0x08, 0x40, 0x02, 0x80, 0x00, 0x01, 0x02, 0x08,
	0x08, 0x40, 0x01, 0x01, 0x40, 0x08, 0x20, 0x04,
	0x10, 0x10, 0x10, 0x10, 0x01, 0x00, 0x02, 0x20,
	0x02, 0x40, 0x04, 0x80, 0x02, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x02, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x01, 0x20, 0x01,
	};

static const uint8_t track2[TRACK_LENGTH] ={0x00,
	0x00, 0x00, 0x04, 0x04, 0x02, 0x80, 0x08, 0x01,
	0x02, 0x20, 0x04, 0x80, 0x00, 0x00, 0x02, 0x08,
	0x02, 0x40, 0x01, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x08, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x08, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x08, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x01, 0x20,
	0x04, 0x40, 0x04, 0x04, 0x40, 0x40, 0x08, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x02, 0x08, 0x00, 0x00};

//track array
static const uint8_t* tracks[] = {track0, track1, track2};
const uint8_t* track;

uint16_t beat;
uint16_t note_array[TRACK_LENGTH];
int score = 0; 
int combo = 0; 
uint16_t beat_counter;

	
//displays the score
void display_terminal_score(void)
{
	move_terminal_cursor(10, 17);
	printf("Game Score: %4d", score);
}	

void display_combo_count(void)
{
	move_terminal_cursor(10, 16);
	printf("Combo: %4d", combo);
}


// Initialise the game by resetting the grid and beat
void initialise_game(void)
{
	// initialise the display we are using.
	default_grid();
	beat = 0;
	display_terminal_score(); 
}

//responsible for updating colour from orange immediently 
void orange_update(void)
{
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		if ((future+beat)%5)
		{
			continue;
		}
		
		uint8_t index = (future+beat)/5;
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		
		for (uint8_t lane=0; lane<4; lane++)
		{
			if (track[index] & (1<<lane))
			{
				PixelColour colour;
				if (note_array[index] == 1)
				{
					colour = COLOUR_GREEN;
				}
				else
				{
					if (combo >= 3)
					{
						colour = COLOUR_ORANGE;
					}
					else
					{
						colour = COLOUR_RED;
					}
				}

				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
		}
	}
}

//responsible for immediately changing colour of ghost notes
void draw_ghost_notes(void)
{
	uint8_t future = 16;

	while (1)
	{
		uint8_t index = (future + beat) / 5;

		if (index >= TRACK_LENGTH)
		{
			break;
		}

		if ((future + beat) % 5)
		{
			future++;
			continue;
		}

		//tracks the next real note, ignores pause notes or long notes
		uint8_t NextTrackValue = track[index] & 0x0F;

		if (NextTrackValue)
		{
			for (uint8_t lane = 0; lane < 4; lane++)
			{
				if (NextTrackValue & (1 << lane))
				{
					PixelColour colour;
					// Check the value of combo and set the color accordingly
					if (combo >= 3)
					{
						colour = COLOUR_DARK_ORANGE;
					}
					else
					{
						colour = COLOUR_DARK_RED;
					}

					// Draw ghost note in the chosen color
					ledmatrix_update_pixel(0, 2 * lane, colour);
					ledmatrix_update_pixel(0, 2 * lane + 1, colour);
				}
			}
			break; // Found a note, exit the loop
		}

		future++;
	}
}


// Play a note in the given lane
void play_note(uint8_t lane)
{	
	// set up an array:
	uint8_t score_decreased[TRACK_LENGTH] = {0};
	for (uint8_t col=11; col<15; col++)
	{
		// col counts from one end, future from the other
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		// notes are only drawn every five columns
		if ((future+beat)%5)
		{
			continue;
		}

		// index of which note in the track to play
		uint8_t index = (future+beat)/5;
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		
		// check if there's a note in the specific path
		if ((track[index] & (1<<lane)))
		{
			
			//decreases if the note was already green
			if (note_array[index] == 1)
			{
				score -= 1;
			}
			
			//if the note is still red, continue
			else
			{
				
				//marks that the notes should be green
				note_array[index] = 1;
				
				//sets it green instantly
				ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
				ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
				
				//sets the audio
				set_audio(lane, col);
				beat_counter = beat; //might be bugged

				if (score_decreased[index] == 0)
				{
					score_decreased[index] = 1;
					
					
					if (col==11 || col==15)
					{
						score += 1;
						combo = 0;
					}
					
					else if (col==12 || col==14)
					{
						score += 2;
						combo = 0;
					}
					
					else if (col == 13)
					{
						if (combo >= 3)
						{
							score += 4;
						}
						
						else
						{
							score += 3;
						}
						combo += 1;
						orange_update();
						draw_ghost_notes();
					}
				}
			}
		}
		else
		{
			score -= 1;
			score_decreased[index] = 1;
			combo = 0;
			orange_update();
			draw_ghost_notes();
		}
	}
	display_terminal_score();
	display_combo_count();
}


// Advance the notes one row down the display
void advance_note(void)
{
	//setting the audio duration. might be bugged
	if (beat >= beat_counter + 5)
	{
		stop_audio();
		beat_counter = 0;
	}
	
	// remove all the current notes; reverse of below
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		// Finish song
		if (index >= TRACK_LENGTH)
		{
/*			for (int i = 0; i<=TRACK_LENGTH; i++)
			{
				note_array[i] = 0; 
			}*/
			break;
		}
		if ((future+beat) % 5)
		{
			continue;
		}
		
		for (uint8_t lane = 0; lane < 4; lane++)
		{
			if (track[index] & (1<<lane))
			{
				PixelColour colour;
				// yellows in the scoring area
				if (col==11 || col == 15)
				{
					colour = COLOUR_QUART_YELLOW;
				}
				else if (col==12 || col == 14)
				{
					colour = COLOUR_HALF_YELLOW;
				}
				else if (col==13)
				{
					colour = COLOUR_YELLOW;
				}
				else
				{
					colour = COLOUR_BLACK;
				}
				
				//determines if there is a note that slid off the screen
				if (note_array[index] != 1 && col == 15)
				{
					score -= 1;
					combo = 0;
					display_terminal_score();
					display_combo_count();
				}
				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
		}
	}
	
	// increment the beat
	beat++;
	
	//ghost notes
	uint8_t future = 16;

	while (1)
	{
		uint8_t index = (future + beat) / 5;

		if (index >= TRACK_LENGTH)
		{
			break;
		}

		if ((future + beat) % 5)
		{
			future++;
			continue;
		}

		//tracks the next real note, ignores pause notes or long notes
		uint8_t NextTrackValue = track[index] & 0x0F;

		if (NextTrackValue)
		{
			for (uint8_t lane = 0; lane < 4; lane++)
			{
				if (NextTrackValue & (1 << lane))
				{
					PixelColour colour;
					//selects colour
					if (combo >= 3)
					{
						colour = COLOUR_DARK_ORANGE; 
					}
					else
					{
						colour = COLOUR_DARK_RED;
					}
					
					// Draw ghost note in dark red
					ledmatrix_update_pixel(0, 2 * lane, colour);
					ledmatrix_update_pixel(0, 2 * lane + 1, colour);
				}
			}
			break; // Found a note, exit the loop
		}

		future++;
	}
	
	// draw the new notes
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		// col counts from one end, future from the other
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		// notes are only drawn every five beats
		if ((future+beat)%5)
		{
			continue;
		}
		
		// index of which note in the track to play
		uint8_t index = (future+beat)/5;
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		// iterate over the four paths
		for (uint8_t lane=0; lane<4; lane++)
		{
			
			// check if there's a note in the specific path
			if (track[index] & (1<<lane))
			{
				PixelColour colour;
				if (note_array[index] == 1)
				{
					colour = COLOUR_GREEN;
				}
				
				else
				{
					//test 
					if (combo >= 3)
					{
						colour = COLOUR_ORANGE;
					}
					else
					{
						colour = COLOUR_RED;
					}				
					
				}

				// if so, colour the two pixels red
				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
		}
	}
}



// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void)
{
	// YOUR CODE HERE
	// Detect if the game is over i.e. if a player has won.
	if (beat/5 >= TRACK_LENGTH)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}


//changing the track 
void change_track(uint8_t track_no)
{
	track = tracks[track_no];
}
