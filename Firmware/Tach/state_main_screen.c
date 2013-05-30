#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "state_main_screen.h"
#include "states.h"
#include "power_monitor.h"
#include "display.h"
#include "tach_monitor.h"
#include "temp_monitor.h"
#include "settings_manager.h"

typedef struct
{
	char first_line_buf[DISPLAY_LINE_SIZE+1];
	char second_line_buf[DISPLAY_LINE_SIZE+1];
	uint8_t main_screen_view_mode;
	one_wire_temperature_data_t tdata;
} main_state_data;

#define MAIN_SCREEN_BASIC_VIEW_MODE 0
#define MAIN_SCREEN_TACH_BASIC_VIEW_MODE 1
#define MAIN_SCREEN_TACH_GRAPH_VIEW_MODE 2

void state_main_screen_state_enter(void **pStateBuf)
{
	main_state_data *pData;
	displayClear();
	*pStateBuf = malloc(sizeof(main_state_data));
	pData = (main_state_data*) *pStateBuf;

	pData->tdata.degree_base = 0;
	pData->tdata.degree_mantissa = 0;
	pData->tdata.is_positive = ONE_WIRE_TEMPERATURE_POSITIVE;
	
	pData->main_screen_view_mode = MAIN_SCREEN_BASIC_VIEW_MODE;
}

void state_main_screen_state_exit(void **pStateBuf)
{
	if (NULL != *pStateBuf)
	{
		free(*pStateBuf);
	}
}


void state_main_screen_state_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	main_state_data *pData = (main_state_data*) *pStateBuf;	
	uint16_t voltage = 0;
	uint8_t temp_conversion_status = TEMP_MONITOR_CONVERSION_IDLE;
	uint16_t rpm = 0;
	uint8_t tach_render_counter = 0;
	uint16_t rpm_delta = 0;
	
	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* Switch view modes */
			pData->main_screen_view_mode = (pData->main_screen_view_mode == MAIN_SCREEN_BASIC_VIEW_MODE ? MAIN_SCREEN_TACH_BASIC_VIEW_MODE : (pData->main_screen_view_mode == MAIN_SCREEN_TACH_BASIC_VIEW_MODE ? MAIN_SCREEN_TACH_GRAPH_VIEW_MODE : MAIN_SCREEN_BASIC_VIEW_MODE));

			/* do not break here to redraw screen immediately */
		case TACH_EVENT_REDRAW_SCREEN:

			if (MAIN_SCREEN_TACH_GRAPH_VIEW_MODE != pData->main_screen_view_mode)
			{
				/* Check temp conversion status */
				temp_conversion_status = temp_monitor_get_conversion_status();
				if (TEMP_MONITOR_CONVERSION_DONE == temp_conversion_status)
				{
					/* Read new temperature from the sensor */
					temp_monitor_get_temperature(&(pData->tdata));
				}
				else if (TEMP_MONITOR_CONVERSION_IDLE == temp_conversion_status)
				{
					/* Initiate new temperature conversion */
					temp_monitor_start_conversion();
				}	
			}
			
			rpm = tach_monitor_get_rpm();
			
			switch (pData->main_screen_view_mode)
			{
				case MAIN_SCREEN_BASIC_VIEW_MODE:
					snprintf(pData->first_line_buf, DISPLAY_LINE_SIZE + 1, "%u об/мин", rpm);
					pData->first_line_buf[DISPLAY_LINE_SIZE] = 0;
					voltage = power_monitor_get_voltage();
					snprintf(pData->second_line_buf,
							DISPLAY_LINE_SIZE+1,
							"%.2u.%.2uV    %c%.2u.%.1uC",
							voltage / 66,
							((voltage % 66) * 151) / 100,
							(pData->tdata.is_positive == ONE_WIRE_TEMPERATURE_POSITIVE ? '+' : '-'),
							pData->tdata.degree_base,
							pData->tdata.degree_mantissa / 1000);
					pData->second_line_buf[DISPLAY_LINE_SIZE] = 0;

					displayPrintLine(pData->first_line_buf, pData->second_line_buf);
					break;
				case MAIN_SCREEN_TACH_BASIC_VIEW_MODE:
					voltage = power_monitor_get_voltage();
					snprintf(pData->second_line_buf,
							DISPLAY_LINE_SIZE+1,
							"%u %.2u.%.1u %c%.2u.%.1u",
							rpm,
							voltage / 66,
							((voltage % 66) * 151) / 1000,
							(pData->tdata.is_positive == ONE_WIRE_TEMPERATURE_POSITIVE ? '+' : '-'),
							pData->tdata.degree_base,
							pData->tdata.degree_mantissa / 1000);
							pData->second_line_buf[DISPLAY_LINE_SIZE] = 0;

					for (tach_render_counter = 0; tach_render_counter < DISPLAY_LINE_SIZE; tach_render_counter++)
					{
						if (rpm < settings_manager_get_min_rpm())
						{
							rpm = settings_manager_get_min_rpm();
						}
						else if (rpm > settings_manager_get_max_rpm())
						{
							rpm = settings_manager_get_max_rpm();
						}
						
						rpm_delta = (settings_manager_get_max_rpm() - settings_manager_get_min_rpm()) / DISPLAY_LINE_SIZE;
						
						pData->first_line_buf[tach_render_counter] = (rpm >= (settings_manager_get_min_rpm() + rpm_delta*tach_render_counter) ? '|' : ' ');
					}							
					
					displayPrintLine(pData->first_line_buf, pData->second_line_buf);
					break;
				case MAIN_SCREEN_TACH_GRAPH_VIEW_MODE:					
					for (tach_render_counter = 0; tach_render_counter < DISPLAY_LINE_SIZE; tach_render_counter++)
					{
						if (rpm < settings_manager_get_min_rpm())
						{
							rpm = settings_manager_get_min_rpm();
						}
						else if (rpm > settings_manager_get_max_rpm())
						{
							rpm = settings_manager_get_max_rpm();
						}
						
						rpm_delta = (settings_manager_get_max_rpm() - settings_manager_get_min_rpm()) / DISPLAY_LINE_SIZE;
						
						pData->first_line_buf[tach_render_counter] = (rpm >= (settings_manager_get_min_rpm() + rpm_delta*tach_render_counter) ? '|' : ' ');
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
			tach_states_schedule_state(TACH_STATE_SETTINGS_PARENT);			
			break;			
		default:
			break;				
	}	
}