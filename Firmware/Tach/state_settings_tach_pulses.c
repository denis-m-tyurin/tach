#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_tach_pulses.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "tach_monitor.h"
#include "settings_manager.h"

const char settings_tach_pulses_str[] PROGMEM = "ÈÌÏÓËÜÑÎÂ/ÎÁÎÐÎÒ";

typedef struct 
{
	char* settings_tach_pulses_str_tmp;
	char out_buf[18];
	uint8_t view_mode;
	uint8_t tmp_pulses_setting;
} settings_tach_pulses_state_strings;

void state_settings_tach_pulses_enter(void **pStateBuf)
{
	settings_tach_pulses_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_tach_pulses_state_strings));
	pData = (settings_tach_pulses_state_strings*) *pStateBuf;
	pData->settings_tach_pulses_str_tmp = utils_read_string_from_progmem(settings_tach_pulses_str);
	pData->view_mode = 1;
	pData->tmp_pulses_setting = settings_manager_get_pulses_per_revolution();
}

void state_settings_tach_pulses_exit(void **pStateBuf)
{
		settings_tach_pulses_state_strings *pData = (settings_tach_pulses_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_tach_pulses_str_tmp)
		{
			free(pData->settings_tach_pulses_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_tach_pulses_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_tach_pulses_state_strings *pData = (settings_tach_pulses_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			pData->view_mode = (pData->view_mode == 0 ? 1 : 0);
			
			if (1 == pData->view_mode)
			{
				/* Switched back from edit mode. Push data to EEPROM */
				settings_manager_set_pulses_per_revolution(pData->tmp_pulses_setting);
			}			
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->out_buf, 18, (pData->view_mode == 1 ? "%u :%uîá/ì" : "<%u> :%uîá/ì"), pData->tmp_pulses_setting, tach_monitor_get_rpm());
			displayPrintLine(pData->settings_tach_pulses_str_tmp, pData->out_buf);
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
				if (pData->tmp_pulses_setting < 100)
				{
					 pData->tmp_pulses_setting++;
					 tach_monitor_init(pData->tmp_pulses_setting);
				}
			}
			
			break;
		case TACH_EVENT_ENCODER_LEFT:
			if (1 == pData->view_mode)
			{
				/* Jump into last settings state */
				tach_states_schedule_state(TACH_STATE_SETTINGS_EXIT);				
			}
			else
			{
				/* otherwise, decrease the counter */
				if (pData->tmp_pulses_setting > 1)
				{
					pData->tmp_pulses_setting--;
					tach_monitor_init(pData->tmp_pulses_setting);
				}
			}			
			break;		
		default:
			break;				
	}	
}