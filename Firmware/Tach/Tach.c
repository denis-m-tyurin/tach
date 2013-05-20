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
#include "settings_manager.h"
#include "tach_monitor.h"

void start_timer0_tach();
void stop_timer0_tach();

/* LOCAL GLOBAL VARS */
uint32_t timer0_count = 0;
uint32_t tach_pulse_count = 0;


int main(void)
{
	uint8_t redraw_cycle = 0; /* Used to count sleep cycles to redraw screen every 0,25 sec */
	TACH_STATE_ID_T scheduled_state; /* Used to store scheduled state */
	
	settings_manager_init();
	power_monitor_init();
	power_monitor_set_voltage_compensation(settings_manager_get_voltage_compensation()); 
	
	encoder_monitor_init();
	
	/* Beeper use the same timer as encoder monitor, therefore
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
	display_set_backlight(BACKLIGHT_TOP);	
	
	/* Configure TOP and SIDE light switch pins */
	DDRA |= (1 << PA7) | (1 << PA6);
	PORTA &= ~((1 << PA6) | (1 << PA7));
	
	
	/* Initialize 1-wire bus */
//	pBus = 	one_wire_initialize_bus(ONE_WIRE_PORT_A, PA4);
	
	/* Show the main screen on start-up */
	tach_states_set_state(TACH_STATE_MAIN_SCREEN);

	sei();
	
	/* Start tach counting */
	/* Enable INT0 interruptions on rising edge */
	tach_monitor_init(settings_manager_get_pulses_per_revolution());
	tach_pulse_count = 0;
	timer0_count = 0;
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	GICR |= (1 << INT0);
	start_timer0_tach();
	
    while(1)
    {		
		
		/* Main loop */
		
		/* Get current state to update screen every 0.25 sec */
		if (25 == redraw_cycle)
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
			//	beeper_play_tone(40);
				tach_states_dispatch_event(TACH_EVENT_ENCODER_RIGHT , NULL);
				break;
			case ENCODER_ACTION_LEFT:
			//	beeper_play_tone(40);
				tach_states_dispatch_event(TACH_EVENT_ENCODER_LEFT , NULL);
				break;
			case ENCODER_ACTION_BUTTON_PRESSED:
			//	beeper_play_tone(400);
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
		
		
	//	snprintf(buf1, 16, "%.2u.%.2uV   %c%d.%.2u    ", voltage/66, voltage % 66, (pTempData->is_positive == ONE_WIRE_TEMPERATURE_POSITIVE ? '+' : '-'), pTempData->degree_base, pTempData->degree_mantissa / 100);
	//snprintf(buf2, 16, "%lu %lu %d, %d           ", RPM_3 / 6, RPM_3 / 8, sound_freq, EncoderData / 4);
	//	displayPrintLine(buf1, buf2);
	
    }
}

/* Timer0 is used to measure timeframe for Tach */
void start_timer0_tach()
{
	/* WGM01=1 - Clear Timer on Compare mode; 64 - prescaler
	 * Will get us 16 000 000 / 64 = 250 000 timer ticks per second (~980 interrupts a second)
	 * So that each 1 tick of the counter would take 0.000004 sec */
	
	TCCR0 = (1 << WGM01) | (1 << CS01) | (1 << CS00);
	OCR0 = 0xFF; // Set MAX to 255
	TIMSK |= (1 << OCIE0); /* Enable output compare match interrupt */
}

void stop_timer0_tach()
{
	TIMSK &= ~(1 << OCIE0); 
	TCCR0 = 0;		
}

ISR(TIMER0_COMP_vect)
{
	timer0_count++;
	
	if (976 <= timer0_count)
	{
		/* 976 = 250 000 / 256
		 * there weren't pulses for a long time, which means RPM is
		 * less than 600 even on engines with 1 pulse per revolution
		 * thus, set RPM to 0 and reset pulse count
		 */
		tach_monitor_reset_rpm();
		tach_pulse_count = 0;
		timer0_count = 0;				
	}
}

ISR(TIMER2_COMP_vect)
{
	encoder_monitor_handle_timer_int();
	beeper_handle_timer_int();
}

ISR(INT0_vect)
{
	/* Tach calculation method:
	 * Min rpm is about 700. Max is 6500
	 * Which is ~11-108 revolutions per second X pulses per revolution
	 * The idea is to measure a timeframe of N pulses
	 * RPM = 60 / (MEASURED_TIME_SEC / N_PULSES) / PULSES_PER_REVOLUTION
	 *
	 * Example: 1000 RPM, 4 pulses per revolution
	 * 
	 * 1000 RPM would be 4000 pulses per minute, or 66.7 pulses per second
	 * 1 pulse would take 0.015 sec	 
	 * 
	 * 1000 RPM = 60 / (0.15 / 10) / 4
	 *
	 * ALGORITHM:
	 * 1. Count ticks
	 * 2. Multiply ticks on 1 tick time
	 * 3. Divide this time on pulses to count
	 * 4. Divide 60 sec on pulses per revolution
	 * 5. RPM = Divide the time from 4th item on time from 3rd
	 */
	
	tach_pulse_count++;
	
	if (tach_pulse_count == TACH_MONITOR_PULSES_TO_PROBE)
	{
		/* Stop Timer 0 */
		stop_timer0_tach();
		
		/* Reset counter */
		tach_pulse_count = 0;
		
		/* Calculate RPM */
		tach_monitor_update_rpm((timer0_count*256) + TCNT0);
		
		/* Start Timer 0 counter */
		timer0_count = 0;
		TCNT0 = 0;
		start_timer0_tach();
	}
	
	
}