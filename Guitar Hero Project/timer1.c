/*
 * timer1.c
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t freq;
float dutycycle;

//responsible for converting the frequency
uint16_t freq_to_clock_period(uint16_t freq)
{
	return (1000000UL / freq);
}

//responsible for converting the dutycycle
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod)
{
	return (dutycycle * clockperiod) / 100;
}

/* Set up timer 1
 */
void init_timer1(void)
{
	TCNT1 = 0;
	
	// Make pin OC1B be an output (port D, pin 4)
	DDRD |= (1<<4);

	//the timer settings 
	TCCR1A = (1 << COM1B1) | (0 <<COM1B0) | (1 <<WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10); 
}

//stop audio 
void stop_audio(void)
{
	OCR1A = 0;
	OCR1B = 0;
}

//set audio
void set_audio(uint8_t lane, uint8_t col)
{
	OCR1A = 1; //might be bugged
	OCR1B = 1;
	
	if (lane == 3) //button 0
	{
		freq = 783.99;
	} 
	
	else if (lane == 2) //button 1
	{
		freq = 698.4565;
	}
	
	else if (lane == 1) //button 2
	{
		freq = 622.254;
	}
	
	else  if (lane == 0)//button 3
	{
		freq = 523.2511;
	}
	
	if (col == 11)
	{
		dutycycle = 2;
	}
	
	else if (col == 12)
	{
		dutycycle = 10;
	}
	
	else if (col == 13)
	{
		dutycycle = 50;
	}
		
	else if (col == 14)
	{
		dutycycle = 90;
	}
	
	else if (col == 15)
	{
		dutycycle = 98;
	}
	
	//adjust the OCR1X values
	uint16_t clockperiod = freq_to_clock_period(freq);
	uint16_t pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
 
	OCR1A = clockperiod - 1;
	
	OCR1B = pulsewidth - 1;
} 
