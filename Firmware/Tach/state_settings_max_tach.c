#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_max_tach.h"
#include "states.h"
#include "utils.h"
#include "display.h"

const char settings_max_tach_str[] PROGMEM = "ÌÀÊÑ. ÎÁÎÐÎÒÎÂ";

static uint8_t view_mode = 1;

static uint8_t tmp_max_tach_setting = 1;
static char tmp_buf[18];

typedef struct 
{
	char* settings_max_tach_str_tmp;
} settings_max_tach_state_strings;

void state_settings_max_tach_enter(void **pStateBuf)
{
	settings_max_tach_state_strings *pStrings;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_max_tach_state_strings));
	pStrings = (settings_max_tach_state_strings*) *pStateBuf;
	pStrings->settings_max_tach_str_tmp = utils_read_string_from_progmem(settings_max_tach_str);
}

void state_settings_max_tach_exit(void **pStateBuf)
{
		settings_max_tach_state_strings *pStrings = (settings_max_tach_state_strings*) *pStateBuf;
		
		if (NULL != pStrings->settings_max_tach_str_tmp)
		{
			free(pStrings->settings_max_tach_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_max_tach_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_max_tach_state_strings *pStrings = (settings_max_tach_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			view_mode = (view_mode == 0 ? 1 : 0);
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(tmp_buf, 18, (view_mode == 1 ? "      %u      " : "     <%u>     "), tmp_max_tach_setting);		
			displayPrintLine(pStrings->settings_max_tach_str_tmp, tmp_buf);
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			if (1 == view_mode)
			{
				/* schedule next state in view mode */
				tach_states_schedule_state(tach_states_get_next_state());	
			}
			else
			{
				/* otherwise, increase the counter */
				tmp_max_tach_setting++;
			}
			
			break;
		case TACH_EVENT_ENCODER_LEFT:
			if (1 == view_mode)
			{
				/* schedule prev state in view mode */
				tach_states_schedule_state(tach_states_get_prev_state());
			}
			else
			{
				/* otherwise, decrease the counter */
				if (tmp_max_tach_setting > 1) tmp_max_tach_setting--;
			}			
			break;		
		default:
			break;				
	}	
}