#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "encoder_monitor.h"

/* Local vars */
static uint8_t EncoderState = 0;
static uint8_t LeftPulses = 0;
static uint8_t RightPulses = 0;
static uint8_t EncoderAction = ENCODER_ACTION_NO_ACTION;
static uint16_t EncoderPressedButtonCounter = 0;
static uint8_t EncoderPressedButtonEventFired = 0;

/* constants */
#define ENCODER_PRESSED_BUTTON_CYCLES_TRESHOLD 100

void encoder_monitor_init()
{
	/* PD0 - encoder button, normally pulled up
	   Encoder A - PD3, B - PD1, normally pulled up */
	
	/* Start Timer2, which is used to analyse state of the connected Encoder  
	   WGM21=1 - Clear Timer on Compare mode; 64 - prescaler, 256 TOP == ~1 kHz */	
	TCCR2 = (1 << WGM21) | (1 << CS22);	
	OCR2 = 0xFF;
	TIMSK |= (1 << OCIE2); /* Enable output compare match interrupt. */
	
	/* !!! Interrupt handler for Timer2 is in tach.c file 
	 * The same handler is used for beeper, therefore sound will work
	 * only together with encoder monitor */	
	
	/* Use the following code to stop Timer2 whenever required ( currently not used ) */
	#if 0
		TIMSK &= ~(1 << OCIE2); 
		TCCR2 = 0;		
	#endif
}

void encoder_monitor_handle_timer_int()
{
	uint8_t enc_current_state;
	/* This is called ~1000 times a second, i.e. 1 kHz
	  * 16 000 000 (16Mhz sys clock) / 64 (pre-scaler) / 256 (top) */
	// Read encoder state
	enc_current_state = PIND & ((1 << PD3) | (1 << PD1));
	switch(EncoderState)
	{
	case 8:
		{
			if(enc_current_state == 10)
			{
				RightPulses++;
				LeftPulses = 0;
			}		
			if(enc_current_state == 0) 
			{
				LeftPulses++;
				RightPulses = 0;
			}			
			break;
		}
 
	case 0:
		{
			if(enc_current_state == 8)
			{
				RightPulses++;
				LeftPulses = 0;
			}				
			if(enc_current_state == 2)
			{
				LeftPulses++;
				RightPulses = 0;
			}				
			break;
		}
	case 2:
		{
			if(enc_current_state == 0)
			{
				RightPulses++;
				LeftPulses = 0;
			}				
			if(enc_current_state == 10)
			{
				LeftPulses++;
				RightPulses = 0;
			}				
			break;
		}
	case 10:
		{
			if(enc_current_state == 2)
			{
				RightPulses++;
				LeftPulses = 0;
			}				
			if(enc_current_state == 8)
			{
				LeftPulses++;
				RightPulses = 0;
			}				
			break;
		}
	}
	
	if(RightPulses == 4)
	{
		EncoderAction = ENCODER_ACTION_RIGHT;
		RightPulses = 0;
		LeftPulses = 0;
	}		
	if(LeftPulses == 4)
	{
		EncoderAction = ENCODER_ACTION_LEFT;
		RightPulses = 0;
		LeftPulses = 0;
	}				
	EncoderState = enc_current_state;
	
	/* check if the button is pressed */
	if (0 == (PIND & (1 << PD0)) )
	{
		if (0 == EncoderPressedButtonEventFired)
		{
			EncoderPressedButtonCounter++;
		
			/* Fire the pressed button event whenever counter exceeds the threshold
			* Skip this cycle if another event has already happened  */
			if ((EncoderPressedButtonCounter > ENCODER_PRESSED_BUTTON_CYCLES_TRESHOLD) && (ENCODER_ACTION_NO_ACTION == EncoderAction))
			{
				EncoderAction = ENCODER_ACTION_BUTTON_PRESSED;
				EncoderPressedButtonCounter = 0;
				EncoderPressedButtonEventFired = 1;
			}
		}			
	}
	else
	{
		EncoderPressedButtonCounter = 0;
		EncoderPressedButtonEventFired = 0;
	}
	
}

uint8_t encoder_monitor_get_last_action()
{
	uint8_t encoder_action = EncoderAction;
	EncoderAction = ENCODER_ACTION_NO_ACTION;
	return encoder_action;
}