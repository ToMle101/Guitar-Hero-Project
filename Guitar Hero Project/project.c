/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett, Cody Burnett
 * Modified by <YOUR NAME HERE>
 */

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"
#include <stdbool.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

bool manual_mode = false;
bool game_pause = false;

uint16_t game_speed;

//variables for the 7 seg
uint8_t seven_seg[11] = { 63,6,91,79,102,109,125,7,127,111,64};
	
//track number 
int track_num = 0;

/////////////////////////////// main //////////////////////////////////
int main(void)
{
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete.
	start_screen();
	
	// Loop forever and continuously play the game.
	while(1)
	{
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void)
{
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200, 0);
	
	init_timer0();
	init_timer1();
	init_timer2();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void)
{
	// Clear terminal screen and output a message
	clear_terminal();
	show_cursor();
	clear_terminal();
	hide_cursor();
	set_display_attribute(FG_WHITE);
	move_terminal_cursor(10,4);
	printf_P(PSTR("  ______   __     __  _______         __    __"));
	move_terminal_cursor(10,5);
	printf_P(PSTR(" /      \\ |  \\   |  \\|       \\       |  \\  |  \\"));
	move_terminal_cursor(10,6);
	printf_P(PSTR("|  $$$$$$\\| $$   | $$| $$$$$$$\\      | $$  | $$  ______    ______    ______"));
	move_terminal_cursor(10,7);
	printf_P(PSTR("| $$__| $$| $$   | $$| $$__| $$      | $$__| $$ /      \\  /      \\  /      \\"));
	move_terminal_cursor(10,8);
	printf_P(PSTR("| $$    $$ \\$$\\ /  $$| $$    $$      | $$    $$|  $$$$$$\\|  $$$$$$\\|  $$$$$$\\"));
	move_terminal_cursor(10,9);
	printf_P(PSTR("| $$$$$$$$  \\$$\\  $$ | $$$$$$$\\      | $$$$$$$$| $$    $$| $$   \\$$| $$  | $$"));
	move_terminal_cursor(10,10);
	printf_P(PSTR("| $$  | $$   \\$$ $$  | $$  | $$      | $$  | $$| $$$$$$$$| $$      | $$__/ $$"));
	move_terminal_cursor(10,11);
	printf_P(PSTR("| $$  | $$    \\$$$   | $$  | $$      | $$  | $$ \\$$     \\| $$       \\$$    $$"));
	move_terminal_cursor(10,12);
	printf_P(PSTR(" \\$$   \\$$     \\$     \\$$   \\$$       \\$$   \\$$  \\$$$$$$$ \\$$        \\$$$$$$"));
	move_terminal_cursor(10,14);
	// change this to your name and student number; remove the chevrons <>
	printf_P(PSTR("CSSE2010/7201 A2 by Bao-Luan Le - s4808720"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	show_start_screen();

	uint32_t last_screen_update, current_time;
	last_screen_update = get_current_time();
	
	uint8_t frame_number = 0;
	game_speed = 1000;

	//show gamespeed 
	move_terminal_cursor(10, 15);
	clear_to_end_of_line();
	printf("Game Speed: %4d %s", game_speed, "Normal");
	
	//show the track
	change_track(0); 
	move_terminal_cursor(10, 16);
	clear_to_end_of_line();
	printf("Selected Track: %d", track_num);

	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1)
	{
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			
			if(serial_input == 'm' || serial_input == 'M') {
				manual_mode = !manual_mode; // toggle manual mode
				if(manual_mode) {
					move_terminal_cursor(10,17);
					clear_to_end_of_line();
					printf("Manual mode activated.\n");
					} else {
					move_terminal_cursor(10,17);
					clear_to_end_of_line();

				}
			}
			
			if(serial_input == '1') 
			{
				game_speed = 1000;
				move_terminal_cursor(10, 15);
				clear_to_end_of_line();
				printf("Game Speed: %4d %s", game_speed, "Normal");	
			}
			
			if(serial_input == '2')
			{
				game_speed = 500;
				move_terminal_cursor(10, 15);
				clear_to_end_of_line();
				printf("Game Speed: %4d %s", game_speed, "Fast");
			}
			
			if(serial_input == '3')
			{
				game_speed = 250;
				move_terminal_cursor(10, 15);
				clear_to_end_of_line();
				printf("Game Speed: %4d %s", game_speed, "Extreme");
			}
			
			//track select
			if(serial_input == 't' || serial_input == 'T')
			{
				track_num = (track_num + 1) % 3;
				
				//call the track change function. make sure to add it to the header file
				change_track(track_num);
				
				//change the text on terminal 
				move_terminal_cursor(10, 16);
				clear_to_end_of_line();
				printf("Selected Track: %d", track_num);

			}

		}
		
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S')
		{
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}

		// every 200 ms, update the animation
		current_time = get_current_time();
		if (current_time - last_screen_update > game_speed/5)
		{
			update_start_screen(frame_number);
			frame_number = (frame_number + 1) % 32;
			last_screen_update = current_time;
		}
	}
}

//countdown function 
static int one[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,1,1,1,1,0,0},
};

static int two[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,1,0,0,1,0,0},
	{0,0,0,0,1,1,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,1,1,1,1,0,0},
};

static int three[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0},
	{0,0,1,1,1,1,1,0},
	{0,1,0,0,1,1,1,0},
	{0,0,0,1,1,1,0,0},
	{0,0,0,1,1,1,0,0},
	{0,0,0,0,0,1,1,0},
	{0,1,1,0,0,1,1,0},
	{0,1,1,1,1,1,0,0},
	{0,0,1,1,1,0,0,0},
};

static int go[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0},
	{1,0,0,0,1,0,0,1},
	{1,0,0,0,1,0,0,1},
	{1,0,0,0,1,0,0,1},
	{1,0,1,0,1,0,0,1},
	{0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};

//test code: 
void countdown(uint16_t game_speed) {
	// Arrays for the numbers are already defined
	int (*numbers[4])[10][8] = {&three, &two, &one, &go};
	for (int num = 0; num < 4; num++) {
		uint32_t last_time = get_current_time();
		ledmatrix_clear();
		while (get_current_time() < last_time + game_speed * 2) {
			// Draw a number
			for(int i=0; i<10; i++){
				for(int j=0; j<8; j++){
					if((*numbers[num])[i][j] == 1){
						ledmatrix_update_pixel(i, j, COLOUR_ORANGE);
					}
				}
			}
		}
	}
	// Clear the LED matrix after the countdown
	ledmatrix_clear();
}


//combo text
void combo_text(void)
{
	set_display_attribute(FG_WHITE);
	move_terminal_cursor(10,4);
	printf_P(PSTR("  ______    ______   __       __  _______    ______   __ "));
	move_terminal_cursor(10,5);
	printf_P(PSTR(" /      \\  /      \\ |  \\     /  \\|       \\  /      \\ |  \\"));
	move_terminal_cursor(10,6);
	printf_P(PSTR("|  $$$$$$\\|  $$$$$$\\| $$\\   /  $$| $$$$$$$\\|  $$$$$$\\| $$"));
	move_terminal_cursor(10,7);
	printf_P(PSTR("| $$   \\$$| $$  | $$| $$$\\ /  $$$| $$__/ $$| $$  | $$| $$"));
	move_terminal_cursor(10,8);
	printf_P(PSTR("| $$      | $$  | $$| $$$$\\  $$$$| $$    $$| $$  | $$| $$"));
	move_terminal_cursor(10,9);
	printf_P(PSTR("| $$   __ | $$  | $$| $$\\$$ $$ $$| $$$$$$$\\| $$  | $$ \\$$"));
	move_terminal_cursor(10,10);
	printf_P(PSTR("| $$__/  \\| $$__/ $$| $$ \\$$$| $$| $$__/ $$| $$__/ $$ __ "));
	move_terminal_cursor(10,11);
	printf_P(PSTR(" \\$$    $$ \\$$    $$| $$  \\$ | $$| $$    $$ \\$$    $$|  \\"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("  \\$$$$$$   \\$$$$$$  \\$$      \\$$ \\$$$$$$$   \\$$$$$$  \\$$"));
}



void delete_combo_text(void)
{
	
	for(int i = 4; i <= 12; i++) 
	{
		move_terminal_cursor(10, i);
		clear_to_end_of_line();
	}

}

void new_game(void)
{
	// Clear the serial terminal
	clear_terminal();
	
	//show the track playing 
	move_terminal_cursor(10, 21);
	printf("Selected Track: %d", track_num);
	
	//the score should be present when the countdown occurs
	move_terminal_cursor(10, 17);
	printf("Game Score: %4d", score);
	
	//shows the game speed 
	move_terminal_cursor(10, 18);
	
	if (game_speed == 1000)
	{
		printf("Game Speed: %4d %s", game_speed, "Normal");
	}
	
	if (game_speed == 500)
	{
		printf("Game Speed: %4d %s", game_speed, "Fast");
	}
	
	if (game_speed == 250)
	{
		printf("Game Speed: %4d %s", game_speed, "Extreme");
	}
	
	//count down
	countdown(game_speed);
	
	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
	
	
}

void display_digit(uint8_t number, uint8_t digit)
{
	PORTC = (PORTC & 0xFE) | digit; //only changes the last bit of port C 
	PORTA = seven_seg[number];	// We assume digit is in range 0 to 9
}

void play_game(void)
{
	
	uint32_t last_advance_time, current_time;
	int8_t btn; // The button pushed
	
	DDRC = (0b00011111); 
	DDRA = (0xFF); //sets up seven seg outputs
	uint8_t digit = 0;
	uint8_t value; 
	
	last_advance_time = get_current_time();
	
	// We play the game until it's over
	while (!is_game_over())
	{
		//test
		if (score >= 100)
		{
			if (digit == 0)
			{
				value = score % 10;
			}
			else
			{
				value = (score / 10) % 10;
			}
		}
		else if ((score >= 10) && (score < 100))
		{
			if (digit == 0)
			{
				value = score % 10;
			}
			else
			{
				value = score / 10;
			}
		}
		else if ((score < 0) && (score >= -10))
		{
			if (digit == 0)
			{
				value = -score;
			}
			else
			{
				value = 10; // Display "-" symbol for negative scores
			}
		}
		else if (score < -9)
		{
			value = 10; // Display "-" symbol for scores less than -9
		}
		else
		{
			digit = 0;
			value = score;
		}
				
		display_digit(value, digit);
		digit = 1 - digit;
		
		//turning on and off combo led
		if (combo == 1)
		{
			PORTC |= (0b00000100);
		}
		
		else if (combo == 2)
		{
			PORTC |= (0b00001100);
		}
		
		else if (combo >= 3)
		{
			PORTC |= (0b00011100);
			combo_text();
			
		}
		
		else if (combo == 0)
		{
			PORTC &= ~(0b00011100);
			delete_combo_text();
		}
		
		
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			
			//pause game 
			if(serial_input == 'p' || serial_input == 'P') {
				game_pause = !game_pause; // toggle pause state
				if(game_pause) {
					move_terminal_cursor(10,19);
					printf("Game Paused\n");
					//turn off sound 
					stop_audio();
					// Turn on LED L7 here
					PORTC |=(0b00000010);
					} else {
					move_terminal_cursor(10,19);
					clear_to_end_of_line(); // This will clear the current line in the terminal
					
					// Turn off LED L7 here
					PORTC &= ~(0b00000010);
				}
			}
			
			if(serial_input == 'm' || serial_input == 'M') {
				manual_mode = !manual_mode; // toggle manual mode
				if(manual_mode) {
					move_terminal_cursor(10,20);
					clear_to_end_of_line();
					printf("Manual mode activated.\n");
					} else {
					move_terminal_cursor(10,20);
					clear_to_end_of_line();

				}
			}
			
			else if(manual_mode && (serial_input == 'n' || serial_input == 'N')) 
			{
				advance_note(); // manually advance note in manual mode
			}
		}
		
		if(game_pause) {
			continue;
		}
				
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		// Checkout the function comment in `buttons.h` and the implementation
		// in `buttons.c`.
		btn = button_pushed();
		
		if (btn == BUTTON0_PUSHED || serial_input == 'f' || serial_input == 'F')
		{
			// If button 0 play the lowest note (right lane)
			play_note(3);
		}
		
		if (btn == BUTTON1_PUSHED || serial_input == 'd' || serial_input == 'D')
		{
			play_note(2);
		}
		
		if (btn == BUTTON2_PUSHED || serial_input == 's' || serial_input == 'S')
		{
			play_note(1);
		}
		
		if (btn == BUTTON3_PUSHED || serial_input == 'a' || serial_input == 'A')
		{
			play_note(0);
		}
		
		current_time = get_current_time();
		if (!manual_mode && current_time >= last_advance_time + game_speed/5)
		{
			// 200ms (0.2 second) has passed since the last time we advance the
			// notes here, so update the advance the notes
			advance_note();
			
			// Update the most recent time the notes were advance
			last_advance_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over()
{
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button or 's'/'S' to start a new game"));

	// Do nothing until a button is pushed. Hint: 's'/'S' should also start a
	// new game
	while (1)
	{
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			if(serial_input == 's' || serial_input == 'S') {
				break;
			}
		}

		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}
	}

	// Return to the splash screen
	start_screen();
}


