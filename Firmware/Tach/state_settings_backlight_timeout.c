#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_backlight_timeout.h"
#include "states.h"
#include "utils.h"
#include "display.h"

const char settings_backlight_timeout_str[] PROGMEM = "ÂÐÅÌß ÏÎÄÑÂÅÒÊÈ";

typedef struct 
{
	char* settings_backlight_timeout_str_tmp;
	char out_buf[18];
	uint8_t view_mode;
	uint8_t tmp_backlight_timeout_setting;

} settings_backlight_timeout_state_strings;

void state_settings_backlight_timeout_enter(void **pStateBuf)
{
	settings_backlight_timeout_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_backlight_timeout_state_strings));
	pData = (settings_backlight_timeout_state_strings*) *pStateBuf;
	pData->settings_backlight_timeout_str_tmp = utils_read_string_from_progmem(settings_backlight_timeout_str);
	pData->view_mode = 1;
	pData->tmp_backlight_timeout_setting = 1;
}

void state_settings_backlight_timeout_exit(void **pStateBuf)
{
		settings_backlight_timeout_state_strings *pData = (settings_backlight_timeout_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_backlight_timeout_str_tmp)
		{
			free(pData->settings_backlight_timeout_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_backlight_timeout_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_backlight_timeout_state_strings *pData = (settings_backlight_timeout_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			pData->view_mode = (pData->view_mode == 0 ? 1 : 0);
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->out_buf, 18, (pData->view_mode == 1 ? "      %u      " : "     <%u>     "), pData->tmp_backlight_timeout_setting);		
			displayPrintLine(pData->settings_backlight_timeout_str_tmp, pData->out_buf);
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			if (1 == pData->view_mode)
			{
				/* schedule next state in view mode */
				tach_states_schedule_state(tach_states_get_next_state());	
			}
			else
			{
				/* otherwise, increase the counter */
				pData->tmp_backlight_timeout_setting++;
			}
			
			break;
		case TACH_EVENT_ENCODER_LEFT:
			if (1 == pData->view_mode)
			{
				/* schedule prev state in view mode */
				tach_states_schedule_state(tach_states_get_prev_state());
			}
			else
			{
				/* otherwise, decrease the counter */
				if (pData->tmp_backlight_timeout_setting > 1) pData->tmp_backlight_timeout_setting--;
			}			
			break;		
		default:
			break;				
	}	
}