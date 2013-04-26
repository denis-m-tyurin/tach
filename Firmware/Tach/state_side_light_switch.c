#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_side_light_switch.h"
#include "states.h"
#include "utils.h"
#include "display.h"

const char side_light_str[] PROGMEM = " ¡Œ–“Œ¬€≈ Œ√Õ»";
const char side_light_off_str[] PROGMEM = "   <¬€ Àﬁ◊≈Õ€>";
const char side_light_on_str[] PROGMEM = "    <√Œ–ﬂ“>   ";

/* Side light switch is PA6 normally pulled down */

static uint8_t sidelight = 0;

typedef struct
{
	char* side_light_str_tmp;
	char* side_light_off_tmp;
	char* side_light_on_tmp;
} sidelight_state_strings;

void state_side_light_switch_enter(void **pStateBuf)
{
	sidelight_state_strings *pStrings;
	displayClear();
	*pStateBuf = malloc(sizeof(sidelight_state_strings));
	pStrings = (sidelight_state_strings*) *pStateBuf;
	pStrings->side_light_str_tmp = utils_read_string_from_progmem(side_light_str);
	pStrings->side_light_off_tmp = utils_read_string_from_progmem(side_light_off_str);
	pStrings->side_light_on_tmp = utils_read_string_from_progmem(side_light_on_str);
}

void state_side_light_switch_exit(void **pStateBuf)
{
		sidelight_state_strings *pStrings = (sidelight_state_strings*) *pStateBuf;
		
		if (NULL != pStrings->side_light_str_tmp)
		{
			free(pStrings->side_light_str_tmp);
		}
		if (NULL != pStrings->side_light_off_tmp)
		{
			free(pStrings->side_light_off_tmp);
		}
		if (NULL != pStrings->side_light_on_tmp)
		{
			free(pStrings->side_light_on_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}	
}


void state_side_light_switch_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	sidelight_state_strings *pStrings = (sidelight_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch state of the top light */
			sidelight = (sidelight == 0 ? 1 : 0);
			if (1 == sidelight)
			{
				PORTA |= (1 << PA6);
			}
			else
			{
				PORTA &= ~(1 << PA6);
			}
		
			/* do not break here to redraw screen immediately */				
		case TACH_EVENT_REDRAW_SCREEN:		
			displayPrintLine(pStrings->side_light_str_tmp, (sidelight == 0 ? pStrings->side_light_off_tmp : pStrings->side_light_on_tmp));
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