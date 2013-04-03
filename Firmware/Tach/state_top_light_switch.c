#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_top_light_switch.h"
#include "states.h"
#include "utils.h"
#include "display.h"

const char top_light_str[] PROGMEM = " “ŒœŒ¬€… Œ√ŒÕ‹";
const char top_light_off_str[] PROGMEM = "   <¬€ Àﬁ◊≈Õ>";
const char top_light_on_str[] PROGMEM = "    <√Œ–»“>   ";

/* Top light switch is PA7 normally pulled down */

static uint8_t toplight = 1;

typedef struct 
{
	char* top_light_str_tmp;
	char* top_light_off_tmp;
	char* top_light_on_tmp;
} toplight_state_strings;

void state_top_light_switch_enter(void **pStateBuf)
{
	toplight_state_strings *pStrings;
	displayClear();	
	*pStateBuf = malloc(sizeof(toplight_state_strings));
	pStrings = (toplight_state_strings*) *pStateBuf;
	pStrings->top_light_str_tmp = utils_read_string_from_progmem(top_light_str);
	pStrings->top_light_off_tmp = utils_read_string_from_progmem(top_light_off_str);
	pStrings->top_light_on_tmp = utils_read_string_from_progmem(top_light_on_str);
}

void state_top_light_switch_exit(void **pStateBuf)
{
		toplight_state_strings *pStrings = (toplight_state_strings*) *pStateBuf;
		
		if (NULL != pStrings->top_light_str_tmp)
		{
			free(pStrings->top_light_str_tmp);
		}
		if (NULL != pStrings->top_light_off_tmp)
		{
			free(pStrings->top_light_off_tmp);
		}
		if (NULL != pStrings->top_light_on_tmp)
		{
			free(pStrings->top_light_on_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_top_light_switch_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	toplight_state_strings *pStrings = (toplight_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch state of the top light */
			toplight = (toplight == 0 ? 1 : 0);
		
			/* do not break here to redraw screen immediatelly */	
		case TACH_EVENT_REDRAW_SCREEN:		
			displayPrintLine(pStrings->top_light_str_tmp, (toplight == 0 ? pStrings->top_light_off_tmp : pStrings->top_light_on_tmp));
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
}