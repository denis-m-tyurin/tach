#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_settings_max_voltage.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "settings_manager.h"

const char settings_max_voltage_str[] PROGMEM = "����. ����������";

typedef struct 
{
	char* settings_max_voltage_str_tmp;
	char out_buf[DISPLAY_LINE_SIZE+1];
	uint8_t view_mode;
	uint16_t tmp_max_voltage_setting;
} settings_max_voltage_state_strings;

void state_settings_max_voltage_enter(void **pStateBuf)
{
	settings_max_voltage_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_max_voltage_state_strings));
	pData = (settings_max_voltage_state_strings*) *pStateBuf;
	pData->settings_max_voltage_str_tmp = utils_read_string_from_progmem(settings_max_voltage_str);
	pData->view_mode = 1;
	pData->tmp_max_voltage_setting = settings_manager_get_max_voltage();
}

void state_settings_max_voltage_exit(void **pStateBuf)
{
		settings_max_voltage_state_strings *pData = (settings_max_voltage_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_max_voltage_str_tmp)
		{
			free(pData->settings_max_voltage_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_max_voltage_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_max_voltage_state_strings *pData = (settings_max_voltage_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			pData->view_mode = (pData->view_mode == 0 ? 1 : 0);
			
			if (1 == pData->view_mode)
			{
				/* Switched back from edit mode. Push data to EEPROM */
				settings_manager_set_max_voltage(pData->tmp_max_voltage_setting);
			}
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->out_buf, DISPLAY_LINE_SIZE+1, (pData->view_mode == 1 ? "   %.2u.%.2uV" : "   <%.2u.%.2u>V"), pData->tmp_max_voltage_setting / 66,((pData->tmp_max_voltage_setting % 66) * 151) / 100);
			pData->out_buf[DISPLAY_LINE_SIZE] = 0;
			displayPrintLine(pData->settings_max_voltage_str_tmp, pData->out_buf);
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
				pData->tmp_max_voltage_setting++;
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
				if (pData->tmp_max_voltage_setting > 1) pData->tmp_max_voltage_setting--;
			}			
			break;		
		default:
			break;				
	}	
}