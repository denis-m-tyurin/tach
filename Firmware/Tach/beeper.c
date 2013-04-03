#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "beeper.h"

static uint16_t remaining_duration = 0;

void beeper_init()
{
	uint16_t rampup_freq = 120;
	/* enable sound (4 kHz)
	 * Fast PWM. MAX ICR1
	 * Tolge OCR1A
	 * prescaller 16 MHz / 64 * 60 */
	DDRD |= (1 << PD4);
	TCCR1A |= (1 << COM1B1) | (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
	
#if 0 // TEMPORARILY SWITCH OFF
	while(rampup_freq > 50)
	{
		ICR1 = rampup_freq;
		OCR1B = rampup_freq / 2;
		_delay_ms(10);
		rampup_freq--;
		
	}
#endif	
	ICR1 = 0;
	OCR1B = 0;	
}

void beeper_play_tone(uint16_t sound_freq, uint16_t duration)
{
	ICR1 = sound_freq;
	OCR1B = sound_freq / 2;
	
	remaining_duration = duration;
}

void beeper_handle_timer_int()
{
	if (0 != remaining_duration)
	{
		remaining_duration--;
		if (0 == remaining_duration)
		{
			ICR1 = 0;
			OCR1B = 0;
		}
	}
}