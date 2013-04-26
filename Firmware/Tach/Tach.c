/*
 * Tach.c
 *
 * Created: 28.07.2012 21:32:26
 *  Author: denis
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "display.h"
#include "one_wire.h"
#include "states.h"
#include "power_monitor.h"
#include "encoder_monitor.h"
#include "beeper.h"

void start_timer0_tach();
void stop_timer0_tach();

/* LOCAL DEFINES */
#define VOLTAGE_COMPENSATION 10
#define BEEPER_FREQ_60 60
#define BEEPER_FREQ_70 70

/* LOCAL GLOBAL VARS */
uint8_t timer_count = 0;
uint32_t tach_pulse_count = 0;
uint32_t total_pulse_count = 0;
uint32_t RPM_3 = 0;

int main(void)
{
	uint8_t redraw_cycle = 0; /* Used to count sleep cycles to redraw screen every 0,5 sec */
	TACH_STATE_ID_T scheduled_state; /* Used to store scheduled state */
	
	power_monitor_init();
	power_monitor_set_voltage_compensation(VOLTAGE_COMPENSATION);
	
	encoder_monitor_init();
	
	/* Beeper used the same timer as encoder monitor, therefore
	 * sound will only work together with encoder monitor and should
	 * be initialized only after encoder */
	beeper_init();
	
	
//	char *buf1 = (char*) malloc(17);
//	char *buf2 = (char*) malloc(17);
//	one_wire_temperature_data_t* pTempData = malloc(sizeof(one_wire_temperature_data_t));
//	uint16_t voltage = 0;
//	uint16_t temperature = 0;
//	uint16_t sound_gen = 0;
//	one_wire_bus_data_t *pBus;
	//uint8_t sound_freq = 0;
	
	/* NEVER PULL-UP PD7 as it is GND'ed */
	
	initDisplay();
	displayClear();
	display_set_backlight(DISPLAY_BACKLIGHT_ON);
	
	/* Configure TOP and SIDE light switch pins */
	DDRA |= (1 << PA7) | (1 << PA6);
	PORTA &= ~((1 << PA6) | (1 << PA7));
	
//	start_timer0(); // Tach siganl monitor
	
	/* Enable INT0 interruptions on rising edge */
//	MCUCR |= (1 << ISC01) | (1 << ISC00);
//	GICR |= (1 << INT0);
	
	/* Initialize 1-wire bus */
//	pBus = 	one_wire_initialize_bus(ONE_WIRE_PORT_A, PA4);
	
	/* Show the main screen on start-up */
	tach_states_set_state(TACH_STATE_MAIN_SCREEN);

	sei();
    while(1)
    {		
		
		/* Main loop */
		
		/* Get current state to update screen every 0.5 sec */
		if (50 == redraw_cycle)
		{
			tach_states_dispatch_event(TACH_EVENT_REDRAW_SCREEN, NULL);	
			redraw_cycle = 0;
		} 
		else 
		{
			redraw_cycle++;
		}
		
		/* Check if any Encoder events happened */
		switch(encoder_monitor_get_last_action())
		{
			case ENCODER_ACTION_RIGHT:
				beeper_play_tone(BEEPER_FREQ_70, 40);
				tach_states_dispatch_event(TACH_EVENT_ENCODER_RIGHT , NULL);
				break;
			case ENCODER_ACTION_LEFT:
				beeper_play_tone(BEEPER_FREQ_70, 40);
				tach_states_dispatch_event(TACH_EVENT_ENCODER_LEFT , NULL);
				break;
			case ENCODER_ACTION_BUTTON_PRESSED:
				beeper_play_tone(BEEPER_FREQ_60, 400);
				tach_states_dispatch_event(TACH_EVENT_ENCODER_BUTTON_PRESSED , NULL);
				break;				
			case ENCODER_ACTION_NO_ACTION:
			default:
				break;
		}
		
		/* Check if any state switch is scheduled */
		scheduled_state = tach_states_get_scheduled_state();
		if (TACH_STATE_NO_STATE != scheduled_state)
		{
			tach_states_set_state(scheduled_state);
			
			/* Immediately redraw the screen to show new state */
			tach_states_dispatch_event(TACH_EVENT_REDRAW_SCREEN, NULL);
			redraw_cycle = 0;
		}
		
		/* sleep 0.01 sec */
		_delay_ms(10);
			
		//sound_freq++;
		//if (sound_freq == 255)
		//{
			//sound_freq = 0;
		//}
	
		//ICR1 = sound_freq;
		//OCR1B = sound_freq / 2;
//		_delay_ms(1000);
		
		/* Re-read temp */
//		if (NULL != pBus)
//		{
	//		cli();
		//	if (one_wire_check_presence(pBus))
//			{
				/* Good news - the sensor is here */
				
				/* Send SKIP ROM [CCh] */
	//			one_wire_send_command(pBus, 0xCC);

				/* Send CONVERT T [44h] */
		//		one_wire_send_command_wait_done(pBus, 0x44, 200);

			//	if (one_wire_check_presence(pBus))
				//{
					/* Send SKIP ROM [CCh] */
//					one_wire_send_command(pBus, 0xCC);

					/* Send READ SCRATCHPAD [BEh] */
					//one_wire_send_command(pBus, 0xBE);

					/* Read data */
					//temperature = one_wire_receive_data(pBus);					
				//}
			//}				
			//sei();
		//}
		//one_wire_temperature_convert(pTempData, temperature);
		
		//tach_states_dispatch_event(TACH_EVENT_REDRAW_SCREEN, NULL);
	//	snprintf(buf1, 16, "%.2u.%.2uV   %c%d.%.2u    ", voltage/66, voltage % 66, (pTempData->is_positive == ONE_WIRE_TEMPERATURE_POSITIVE ? '+' : '-'), pTempData->degree_base, pTempData->degree_mantissa / 100);
	//snprintf(buf2, 16, "%lu %lu %d, %d           ", RPM_3 / 6, RPM_3 / 8, sound_freq, EncoderData / 4);
	//	displayPrintLine(buf1, buf2);
		
#if 0
		/* Go to sleep */
		MCUCR |= (1 << 7) | (1 << 5);		
		displayPrintLine("Going to sleep       ", "Bye!!!             ");
		sleep_mode();		
#endif		
		
    }
}

/* Timer0 is used to count Tach impulses */
void start_timer0_tach()
{
	/* WGM01=1 - Clear Timer on Compare mode; 1024 - prescaler */	
	TCCR0 = (1 << WGM01) | (1 << CS02) | (1 << CS00);
	OCR0 = 0xFF;
	TIMSK |= (1 << OCIE0); /* Enable output compare match interrupt */
}

void stop_timer0_tach()
{
	TIMSK &= ~(1 << OCIE0); 
	TCCR0 = 0;		
}

ISR(TIMER0_COMP_vect)
{
	/* This is called 61 times a second, i.e. 61 Hz
	  * 16 000 000 (16Mhz sys clock) / 1024 (pre-scaler) / 256 (top) = 61 */

	timer_count++;	

	if (timer_count == 61)
	{
		RPM_3 = tach_pulse_count * 60;
		total_pulse_count += tach_pulse_count;
		tach_pulse_count = 0;
		timer_count = 0;		
	}
}

ISR(TIMER2_COMP_vect)
{
	encoder_monitor_handle_timer_int();
	beeper_handle_timer_int();
}

ISR(INT0_vect)
{
	tach_pulse_count++;	
}