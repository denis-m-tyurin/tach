#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_max_rpm_alarm_on.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "settings_manager.h"

const char settings_max_rpm_alarm_on_str1[] PROGMEM = "ÌAX ÎÁÎÐ ÑÈÃÍÀË";
const char settings_max_rpm_alarm_on_str[] PROGMEM = "âêëþ÷åí";
const char settings_max_rpm_alarm_off_str[] PROGMEM = "âûêëþ÷åí";

typedef struct 
{
	char* settings_max_rpm_alarm_on_str1_tmp;
	char* settings_max_rpm_alarm_on_str_tmp;
	char* settings_max_rpm_alarm_off_str_tmp;	
	char out_buf[DISPLAY_LINE_SIZE+1];	
	uint8_t tmp_max_rpm_alarm_on_setting;

} settings_max_rpm_alarm_on_state_strings;

void state_settings_max_rpm_alarm_on_enter(void **pStateBuf)
{
	settings_max_rpm_alarm_on_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_max_rpm_alarm_on_state_strings));
	pData = (settings_max_rpm_alarm_on_state_strings*) *pStateBuf;
	pData->settings_max_rpm_alarm_on_str1_tmp = utils_read_string_from_progmem(settings_max_rpm_alarm_on_str1);
	pData->settings_max_rpm_alarm_on_str_tmp = utils_read_string_from_progmem(settings_max_rpm_alarm_on_str);
	pData->settings_max_rpm_alarm_off_str_tmp = utils_read_string_from_progmem(settings_max_rpm_alarm_off_str);

	pData->tmp_max_rpm_alarm_on_setting = settings_manager_get_max_rpm_alarm_on();
}

void state_settings_max_rpm_alarm_on_exit(void **pStateBuf)
{
		settings_max_rpm_alarm_on_state_strings *pData = (settings_max_rpm_alarm_on_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_max_rpm_alarm_on_str1_tmp)
		{
			free(pData->settings_max_rpm_alarm_on_str1_tmp);
		}
		if (NULL != pData->settings_max_rpm_alarm_on_str_tmp)
		{
			free(pData->settings_max_rpm_alarm_on_str_tmp);
		}		
		if (NULL != pData->settings_max_rpm_alarm_off_str_tmp)
		{
			free(pData->settings_max_rpm_alarm_off_str_tmp);
		}		
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}

void state_settings_max_rpm_alarm_on_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_max_rpm_alarm_on_state_strings *pData = (settings_max_rpm_alarm_on_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:			
			pData->tmp_max_rpm_alarm_on_setting = (pData->tmp_max_rpm_alarm_on_setting == SETTINGS_MANAGER_ALARM_ENABLED ? SETTINGS_MANAGER_ALARM_DISABLED : SETTINGS_MANAGER_ALARM_ENABLED);
			
			settings_manager_set_max_rpm_alarm_on(pData->tmp_max_rpm_alarm_on_setting);
					
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->out_buf, 
			         DISPLAY_LINE_SIZE+1,
					 "%s",
					 (pData->tmp_max_rpm_alarm_on_setting == SETTINGS_MANAGER_ALARM_ENABLED ? pData->settings_max_rpm_alarm_on_str_tmp : pData->settings_max_rpm_alarm_off_str_tmp));
			pData->out_buf[DISPLAY_LINE_SIZE] = 0;
			displayPrintLine(pData->settings_max_rpm_alarm_on_str1_tmp, pData->out_buf);
					
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