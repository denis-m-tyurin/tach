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

typedef struct
{
	char first_line_buf[18];
	char second_line_buf[18];	
} main_state_data;

void state_main_screen_state_enter(void **pStateBuf)
{
	displayClear();
	*pStateBuf = malloc(sizeof(main_state_data));
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
	
	switch (event)
	{
		case TACH_EVENT_REDRAW_SCREEN:
			snprintf(pData->first_line_buf, 16, "%u об/мин    ", tach_monitor_get_rpm());
			voltage = power_monitor_get_voltage();
			snprintf(pData->second_line_buf, 16, "%.2u.%.2uV    ", voltage / 66, ((voltage % 66) * 151) / 100);			
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