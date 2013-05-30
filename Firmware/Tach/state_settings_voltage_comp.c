#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_voltage_comp.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "power_monitor.h"
#include "settings_manager.h"

const char settings_voltage_comp_str[] PROGMEM = "ÏÎÄÑÒÐÎÉÊÀ ÂÎËÜÒ";

typedef struct 
{
	char* settings_voltage_comp_str_tmp;
	char out_buf[DISPLAY_LINE_SIZE+1];
	uint8_t view_mode;
	int16_t voltage_compensation_setting;
} settings_voltage_comp_state_strings;

void state_settings_voltage_comp_enter(void **pStateBuf)
{
	settings_voltage_comp_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_voltage_comp_state_strings));
	pData = (settings_voltage_comp_state_strings*) *pStateBuf;
	pData->settings_voltage_comp_str_tmp = utils_read_string_from_progmem(settings_voltage_comp_str);
	pData->view_mode = 1;
	pData->voltage_compensation_setting = settings_manager_get_voltage_compensation();
}

void state_settings_voltage_comp_exit(void **pStateBuf)
{
		settings_voltage_comp_state_strings *pData = (settings_voltage_comp_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_voltage_comp_str_tmp)
		{
			free(pData->settings_voltage_comp_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}

void state_settings_voltage_comp_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_voltage_comp_state_strings *pData = (settings_voltage_comp_state_strings*) *pStateBuf;
	uint16_t voltage = 0;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			pData->view_mode = (pData->view_mode == 0 ? 1 : 0);
			
			if (1 == pData->view_mode)
			{
				/* Switched back from edit mode. Push data to EEPROM */
				settings_manager_set_voltage_compensation(pData->voltage_compensation_setting);
			}
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			voltage = power_monitor_get_voltage();
			snprintf(pData->out_buf, DISPLAY_LINE_SIZE+1, (pData->view_mode == 1 ? "%.2u.%.2uV   %+i" : "%.2u.%.2uV   <%+i>"), voltage / 66 , ((voltage % 66) * 151) / 100, pData->voltage_compensation_setting);		
			pData->out_buf[DISPLAY_LINE_SIZE] = 0;
			displayPrintLine(pData->settings_voltage_comp_str_tmp, pData->out_buf);
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
				pData->voltage_compensation_setting++;
				power_monitor_set_voltage_compensation(pData->voltage_compensation_setting);
			}
			
			break;
		case TACH_EVENT_ENCODER_LEFT:
			if (1 == pData->view_mode)
			{
				/* Schedule prev state */
				tach_states_schedule_state(tach_states_get_prev_state());				
			}
			else
			{
				/* otherwise, decrease the counter */
				pData->voltage_compensation_setting--;
				power_monitor_set_voltage_compensation(pData->voltage_compensation_setting);
			}			
			break;		
		default:
			break;				
	}	
}