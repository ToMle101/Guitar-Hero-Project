/*
 * timer1.h
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include <stdint.h>

/* Set up our timer 
 */
void init_timer1(void);


#endif /* TIMER1_H_ */

//including all the functions relating to the audio
uint16_t freq_to_clock_period(uint16_t freq);

uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod);

void stop_audio(void);

void set_audio(uint8_t lane, uint8_t col);
