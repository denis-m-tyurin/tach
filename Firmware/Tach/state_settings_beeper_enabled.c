#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_beeper_enabled.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "settings_manager.h"

const char settings_beeper_enabled_str1[] PROGMEM = "ÇÂÓÊÎÂÎÉ ÑÈÃÍÀË";
const char settings_beeper_on_str[] PROGMEM = "âêëþ÷åí";
const char settings_beeper_off_str[] PROGMEM = "âûêëþ÷åí";

typedef struct 
{
	char* settings_beeper_enabled_str1_tmp;
	char* settings_beeper_on_str_tmp;
	char* settings_beeper_off_str_tmp;	
	char out_buf[DISPLAY_LINE_SIZE+1];	
	uint8_t tmp_beeper_enabled_setting;

} settings_beeper_enabled_state_strings;

void state_settings_beeper_enabled_enter(void **pStateBuf)
{
	settings_beeper_enabled_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_beeper_enabled_state_strings));
	pData = (settings_beeper_enabled_state_strings*) *pStateBuf;
	pData->settings_beeper_enabled_str1_tmp = utils_read_string_from_progmem(settings_beeper_enabled_str1);
	pData->settings_beeper_on_str_tmp = utils_read_string_from_progmem(settings_beeper_on_str);
	pData->settings_beeper_off_str_tmp = utils_read_string_from_progmem(settings_beeper_off_str);

	pData->tmp_beeper_enabled_setting = settings_manager_get_beeper_enabled();
}

void state_settings_beeper_enabled_exit(void **pStateBuf)
{
		settings_beeper_enabled_state_strings *pData = (settings_beeper_enabled_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_beeper_enabled_str1_tmp)
		{
			free(pData->settings_beeper_enabled_str1_tmp);
		}
		if (NULL != pData->settings_beeper_on_str_tmp)
		{
			free(pData->settings_beeper_on_str_tmp);
		}		
		if (NULL != pData->settings_beeper_off_str_tmp)
		{
			free(pData->settings_beeper_off_str_tmp);
		}		
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}

void state_settings_beeper_enabled_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_beeper_enabled_state_strings *pData = (settings_beeper_enabled_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:			
			pData->tmp_beeper_enabled_setting = (pData->tmp_beeper_enabled_setting == SETTINGS_MANAGER_BEEPER_ENABLED ? SETTINGS_MANAGER_BEEPER_DISABLED : SETTINGS_MANAGER_BEEPER_ENABLED);
			
			settings_manager_set_beeper_enabled(pData->tmp_beeper_enabled_setting);
					
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->out_buf, 
			         DISPLAY_LINE_SIZE+1,
					 "%s",
					 (pData->tmp_beeper_enabled_setting == SETTINGS_MANAGER_BEEPER_ENABLED ? pData->settings_beeper_on_str_tmp : pData->settings_beeper_off_str_tmp));
			pData->out_buf[DISPLAY_LINE_SIZE] = 0;
			displayPrintLine(pData->settings_beeper_enabled_str1_tmp, pData->out_buf);
					
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			tach_states_schedule_state(tach_states_get_next_state());	
			break;
		case TACH_EVENT_ENCODER_LEFT:
			tach_states_schedule_state(tach_states_get_prev_state());			
			break;		
		default:
			break;				
	}	
}