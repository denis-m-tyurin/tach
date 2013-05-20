#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_backlight_intensity.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "settings_manager.h"

const char settings_backlight_intensity_str[] PROGMEM = "    ßÐÊÎÑÒÜ";

typedef struct 
{
	char* settings_backlight_intensity_str_tmp;
	char tmp_buf[18];
	uint8_t view_mode;
	uint8_t tmp_backlight_intensity_setting;	
} settings_backlight_intensity_state_strings;

void state_settings_backlight_intensity_enter(void **pStateBuf)
{
	settings_backlight_intensity_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_backlight_intensity_state_strings));
	pData = (settings_backlight_intensity_state_strings*) *pStateBuf;
	pData->settings_backlight_intensity_str_tmp = utils_read_string_from_progmem(settings_backlight_intensity_str);
	pData->view_mode = 1;
	pData->tmp_backlight_intensity_setting = settings_manager_get_backlight_intensity();
}

void state_settings_backlight_intensity_exit(void **pStateBuf)
{
		settings_backlight_intensity_state_strings *pData = (settings_backlight_intensity_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_backlight_intensity_str_tmp)
		{
			free(pData->settings_backlight_intensity_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_backlight_intensity_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_backlight_intensity_state_strings *pData = (settings_backlight_intensity_state_strings*) *pStateBuf;
	uint8_t tmp_counter = 0;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			pData->view_mode = (pData->view_mode == 0 ? 1 : 0);
			
			if (1 == pData->view_mode)
			{
				/* Switched back from edit mode. Push data to EEPROM */
				settings_manager_set_backlight_intensity(pData->tmp_backlight_intensity_setting);
			}
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			//snprintf(pData->tmp_buf, 18, (pData->view_mode == 1 ? "      %u      " : "     <%u>     "), pData->tmp_backlight_intensity_setting);
			
			for (tmp_counter = 1; tmp_counter < 15; tmp_counter++)
			{				
				pData->tmp_buf[tmp_counter] = (  tmp_counter * 5 <= pData->tmp_backlight_intensity_setting ? '|' : ' ');
			}
			pData->tmp_buf[0] = (0 == pData->view_mode ? '<' : ' ');
			pData->tmp_buf[15] = (0 == pData->view_mode ? '>' : ' ');

			pData->tmp_buf[16] = 0;
			
			displayPrintLine(pData->settings_backlight_intensity_str_tmp, pData->tmp_buf);
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
				if (pData->tmp_backlight_intensity_setting < 70)
				{
					pData->tmp_backlight_intensity_setting += 5;
					display_set_backlight(pData->tmp_backlight_intensity_setting);
				}
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
				if (pData->tmp_backlight_intensity_setting > 1) 
				{
					pData->tmp_backlight_intensity_setting -= 5;
					display_set_backlight(pData->tmp_backlight_intensity_setting);
				}
			}			
			break;		
		default:
			break;				
	}	
}