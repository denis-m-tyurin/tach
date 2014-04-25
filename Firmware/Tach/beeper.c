#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "beeper.h"

static uint16_t remaining_duration = 0;

void beeper_init()
{
	/* enable sound (4 kHz)
	 * Fast PWM. MAX ICR1
	 * Togle OCR1A
	 * prescaller 16 MHz / 64 * 60 */
	DDRD |= (1 << PD4);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
	
	ICR1 = 70;
	OCR1B = 0;	
}

void beeper_play_tone(uint16_t duration)
{
	OCR1B = 35;
	TCCR1A |= (1 << COM1B1);
	remaining_duration = duration;		
}

void beeper_handle_timer_int()
{
	if (0 != remaining_duration)
	{
		remaining_duration--;
		if (0 == remaining_duration)
		{
			TCCR1A &= ~(1 << COM1B1);
			OCR1B = 0;
			
		}
	}
}