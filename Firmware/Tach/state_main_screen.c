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

typedef struct
{
	char first_line_buf[18];
	char second_line_buf[18];
} main_state_data;

static one_wire_temperature_data_t tdata;

void state_main_screen_state_enter(void **pStateBuf)
{
	displayClear();
	*pStateBuf = malloc(sizeof(main_state_data));

	tdata.degree_base = 0;
	tdata.degree_mantissa = 0;
	tdata.is_positive = ONE_WIRE_TEMPERATURE_POSITIVE;
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
	
	switch (event)
	{
		case TACH_EVENT_REDRAW_SCREEN:

			/* Check temp conversion status */
			temp_conversion_status = temp_monitor_get_conversion_status();
			if (TEMP_MONITOR_CONVERSION_DONE == temp_conversion_status)
			{
				/* Read new temperature from the sensor */
				temp_monitor_get_temperature(&tdata);
			} 
			else if (TEMP_MONITOR_CONVERSION_IDLE == temp_conversion_status)
			{
				/* Initiate new temperature conversion */
				temp_monitor_start_conversion();
			}

			snprintf(pData->first_line_buf, 16, "%u об/мин    ", tach_monitor_get_rpm());
			voltage = power_monitor_get_voltage();
			snprintf(pData->second_line_buf, 
					 16,
					 "%.2u.%.2uV   %c%.2u.%.2uC",
					 voltage / 66,
					 ((voltage % 66) * 151) / 100,
					 (tdata.is_positive == ONE_WIRE_TEMPERATURE_POSITIVE ? '+' : '-'),
					 tdata.degree_base,
					 tdata.degree_mantissa / 100);

			displayPrintLine(pData->first_line_buf, pData->second_line_buf);

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