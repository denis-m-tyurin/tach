#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_parent.h"
#include "states.h"
#include "utils.h"
#include "display.h"

const char settings_str[] PROGMEM = "   Õ¿—“–Œ… »";

void state_settings_parent_enter(void **pStateBuf)
{
	*pStateBuf = utils_read_string_from_progmem(settings_str);
	displayClear();
}

void state_settings_parent_exit(void **pStateBuf)
{
	if (NULL != *pStateBuf)
	{
		free(*pStateBuf);
	}
}

void state_settings_parent_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	char* settings_str_tmp = (char*) *pStateBuf;
	
	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* jump into the first settings sub-item */
			tach_states_schedule_state(tach_states_get_next_state());		
			break;
		case TACH_EVENT_REDRAW_SCREEN:		
			displayPrintLine(settings_str_tmp, " ");
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			/* Jump to the first state */
			tach_states_schedule_state(TACH_STATE_MAIN_SCREEN);
			break;
		case TACH_EVENT_ENCODER_LEFT:
			/* Schedule prev state */
			tach_states_schedule_state(tach_states_get_prev_state());		
			break;		
		default:
			break;				
	}	
}