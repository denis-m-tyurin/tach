#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include "state_voltage_advanced.h"
#include "states.h"
#include "display.h"

void state_voltage_advanced_state_enter(void **pStateBuf)
{
	displayClear();
}

void state_voltage_advanced_state_exit(void **pStateBuf)
{
	
}


void state_voltage_advanced_state_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	switch (event)
	{
		case TACH_EVENT_REDRAW_SCREEN:
			displayPrintLine("STATE", "VOLTAGE ADVANCED");			
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			/* Schedule next state */
			tach_states_schedule_state(tach_states_get_next_state());
			break;
		case TACH_EVENT_ENCODER_LEFT:
			/* Schedule settings parent state */
			tach_states_schedule_state(tach_states_get_prev_state());
			break;			
		default:
			break;				
	}	
}