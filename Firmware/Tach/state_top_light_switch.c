#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_top_light_switch.h"
#include "states.h"
#include "utils.h"

PGM_P top_light_str = "������� �����";

void state_top_light_switch_enter(void *pStateBuf)
{
	displayClear();
}

void state_top_light_switch_exit(void *pStateBuf)
{
	
}


void state_top_light_switch_event_handler(uint8_t event, void *pStateBuf)
{	
	char* top_light_str_tmp = utils_read_string_from_progmem(top_light_str);
	switch (event)
	{
		case TACH_EVENT_REDRAW_SCREEN:		
			displayPrintLine(top_light_str_tmp, "����");			
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			/* Schedule next state */
			tach_states_schedule_state(tach_states_get_next_state());
			break;
		case TACH_EVENT_ENCODER_LEFT:
			/* Schedule prev state */
			tach_states_schedule_state(tach_states_get_prev_state());
		
			break;		
		default:
			break;				
	}	
	
	if (NULL != top_light_str_tmp) 
	{
		free(top_light_str_tmp);
	}
}