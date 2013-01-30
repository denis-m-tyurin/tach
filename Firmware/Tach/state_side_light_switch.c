#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_side_light_switch.h"
#include "states.h"
#include "utils.h"

char side_light_str[] PROGMEM = " ¡Œ–“Œ¬€≈ Œ√Õ»";
char side_light_off_str[] PROGMEM = "   <¬€ Àﬁ◊≈Õ€>";
char side_light_on_str[] PROGMEM = "    <√Œ–ﬂ“>  ";

/* Side light switch is PA6 normally pulled down */

static uint8_t sidelight = 1;

void state_side_light_switch_enter(void *pStateBuf)
{
	displayClear();
}

void state_side_light_switch_exit(void *pStateBuf)
{
	
}


void state_side_light_switch_event_handler(uint8_t event, void *pStateBuf)
{	
	char* side_light_str_tmp = utils_read_string_from_progmem(side_light_str);
	char* side_light_off_tmp = utils_read_string_from_progmem(side_light_off_str);
	char* side_light_on_tmp = utils_read_string_from_progmem(side_light_on_str);
	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch state of the top light */
			sidelight = (sidelight == 0 ? 1 : 0);
		
			/* do not break here to redraw screen immediately */				
		case TACH_EVENT_REDRAW_SCREEN:		
			displayPrintLine(side_light_str_tmp, (sidelight == 0 ? side_light_off_tmp : side_light_on_tmp));
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
	
	if (NULL != side_light_str_tmp) 
	{
		free(side_light_str_tmp);
	}
	if (NULL != side_light_off_tmp) 
	{
		free(side_light_off_tmp);
	}
	if (NULL != side_light_on_tmp) 
	{
		free(side_light_on_tmp);
	}		
}