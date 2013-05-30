#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include "state_voltage_advanced.h"
#include "states.h"
#include "display.h"
#include "power_monitor.h"
#include "settings_manager.h"

typedef struct
{
	char first_line_buf[DISPLAY_LINE_SIZE+1];
	char second_line_buf[DISPLAY_LINE_SIZE+1];
	uint8_t screen_view_mode;	
} voltage_advanced_state_data;

#define VOLTAGE_SCREEN_BASIC_VIEW_MODE 0
#define VOLTAGE_SCREEN_GRAPH_VIEW_MODE 1

void state_voltage_advanced_state_enter(void **pStateBuf)
{
	voltage_advanced_state_data *pData;
	displayClear();
	
	*pStateBuf = malloc(sizeof(voltage_advanced_state_data));
	pData = (voltage_advanced_state_data*) *pStateBuf;

	pData->screen_view_mode = VOLTAGE_SCREEN_BASIC_VIEW_MODE;
}

void state_voltage_advanced_state_exit(void **pStateBuf)
{
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_voltage_advanced_state_event_handler(uint8_t event, void **pStateBuf, void *data)
{
	voltage_advanced_state_data *pData = (voltage_advanced_state_data*) *pStateBuf;
	uint16_t voltage = 0;
	uint16_t voltage_delta = 0;
	uint8_t voltage_render_counter = 0;
	
	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view modes */
			pData->screen_view_mode = (pData->screen_view_mode == VOLTAGE_SCREEN_BASIC_VIEW_MODE ? VOLTAGE_SCREEN_GRAPH_VIEW_MODE : VOLTAGE_SCREEN_BASIC_VIEW_MODE);
			
			/* do not break here to redraw screen immediately */
		case TACH_EVENT_REDRAW_SCREEN:
			voltage = power_monitor_get_voltage();
			
			switch (pData->screen_view_mode)
			{
				case VOLTAGE_SCREEN_BASIC_VIEW_MODE:
				
					snprintf(pData->second_line_buf,
							DISPLAY_LINE_SIZE+1,
							"%.2u.%.2uV",							
							voltage / 66,
							((voltage % 66) * 151) / 1000);
					pData->second_line_buf[DISPLAY_LINE_SIZE] = 0;
				
					for (voltage_render_counter = 0; voltage_render_counter < DISPLAY_LINE_SIZE; voltage_render_counter++)
					{
						if (voltage < settings_manager_get_min_voltage())
						{
							voltage = settings_manager_get_min_voltage();
						}
						else if (voltage > settings_manager_get_max_voltage())
						{
							voltage = settings_manager_get_max_voltage();
						}
					
						voltage_delta = (settings_manager_get_max_voltage() - settings_manager_get_min_voltage()) / DISPLAY_LINE_SIZE;
					
						pData->first_line_buf[voltage_render_counter] = (voltage >= (settings_manager_get_min_voltage() + voltage_delta*voltage_render_counter) ? '|' : ' ');
					}				
		
					displayPrintLine(pData->first_line_buf, pData->second_line_buf);
					break;
				case VOLTAGE_SCREEN_GRAPH_VIEW_MODE:
					for (voltage_render_counter = 0; voltage_render_counter < DISPLAY_LINE_SIZE; voltage_render_counter++)
					{
						if (voltage < settings_manager_get_min_voltage())
						{
							voltage = settings_manager_get_min_voltage();
						}
						else if (voltage > settings_manager_get_max_voltage())
						{
							voltage = settings_manager_get_max_voltage();
						}
						
						voltage_delta = (settings_manager_get_max_voltage() - settings_manager_get_min_voltage()) / DISPLAY_LINE_SIZE;
						
						pData->first_line_buf[voltage_render_counter] = (voltage >= (settings_manager_get_min_voltage() + voltage_delta*voltage_render_counter) ? '|' : ' ');
					}
					
					displayPrintLine(pData->first_line_buf, pData->first_line_buf);
					break;
			}
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			/* Schedule next state */
			tach_states_schedule_state(tach_states_get_next_state());
			break;
		case TACH_EVENT_ENCODER_LEFT:
			/* Schedule settings parent state */
			tach_states_schedule_state(tach_states_get_prev_state());
			break;			
		default:
			break;				
	}	
}