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

const char settings_voltage_comp_str[] PROGMEM = "ÏÎÄÑÒÐÎÉÊÀ ÂÎËÜÒ";

static uint8_t view_mode = 1;

static int16_t voltage_compensation_setting = 1;

typedef struct 
{
	char* settings_voltage_comp_str_tmp;
	char* out_buf;
} settings_voltage_comp_state_strings;

void state_settings_voltage_comp_enter(void **pStateBuf)
{
	settings_voltage_comp_state_strings *pStrings;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_voltage_comp_state_strings));
	pStrings = (settings_voltage_comp_state_strings*) *pStateBuf;
	pStrings->settings_voltage_comp_str_tmp = utils_read_string_from_progmem(settings_voltage_comp_str);
	pStrings->out_buf = malloc(sizeof(char)*18);
}

void state_settings_voltage_comp_exit(void **pStateBuf)
{
		settings_voltage_comp_state_strings *pStrings = (settings_voltage_comp_state_strings*) *pStateBuf;
		
		if (NULL != pStrings->settings_voltage_comp_str_tmp)
		{
			free(pStrings->settings_voltage_comp_str_tmp);
		}
		if (NULL != pStrings->out_buf)
		{
			free(pStrings->out_buf);
		}		
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}

void state_settings_voltage_comp_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_voltage_comp_state_strings *pStrings = (settings_voltage_comp_state_strings*) *pStateBuf;
	uint16_t voltage = 0;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view/enter mode */
			view_mode = (view_mode == 0 ? 1 : 0);
		
			/* do not break here to redraw screen immediately */	
		case TACH_EVENT_REDRAW_SCREEN:
			voltage = power_monitor_get_voltage();
			snprintf(pStrings->out_buf, 18, (view_mode == 1 ? "%.2u.%.2uV   %+i         " : "%.2u.%.2uV   <%+i>        "), voltage / 66 , ((voltage % 66) * 151) / 100, voltage_compensation_setting);		
			displayPrintLine(pStrings->settings_voltage_comp_str_tmp, pStrings->out_buf);
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
				voltage_compensation_setting++;
				power_monitor_set_voltage_compensation(voltage_compensation_setting);
			}
			
			break;
		case TACH_EVENT_ENCODER_LEFT:
			if (1 == view_mode)
			{
				/* Schedule prev state */
				tach_states_schedule_state(tach_states_get_prev_state());				
			}
			else
			{
				/* otherwise, decrease the counter */
				voltage_compensation_setting--;
				power_monitor_set_voltage_compensation(voltage_compensation_setting);
			}			
			break;		
		default:
			break;				
	}	
}