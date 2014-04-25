#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "state_alarm_max_rpm.h"
#include "states.h"
#include "utils.h"
#include "display.h"
#include "settings_manager.h"
#include "alarm_monitor.h"
#include "tach_monitor.h"
#include "beeper.h"

const char alarm_max_rpm_str[] PROGMEM = "ÂÛÑÎÊÈÅ ÎÁÎÐÎÒÛ";

typedef struct 
{
	char* alarm_max_rpm_str_tmp;
	char out_buf[DISPLAY_LINE_SIZE+1];	
} alarm_max_rpm_state_data;

void state_alarm_max_rpm_enter(void **pStateBuf)
{
	alarm_max_rpm_state_data *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(alarm_max_rpm_state_data));
	pData = (alarm_max_rpm_state_data*) *pStateBuf;
	pData->alarm_max_rpm_str_tmp = utils_read_string_from_progmem(alarm_max_rpm_str);
}

void state_alarm_max_rpm_exit(void **pStateBuf)
{
		alarm_max_rpm_state_data *pData = (alarm_max_rpm_state_data*) *pStateBuf;
		
		if (NULL != pData->alarm_max_rpm_str_tmp)
		{
			free(pData->alarm_max_rpm_str_tmp);
		}
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}

void state_alarm_max_rpm_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	alarm_max_rpm_state_data *pData = (alarm_max_rpm_state_data*) *pStateBuf;
	uint16_t rpm;
	uint16_t max_rpm;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			/* return to the previous state */
			alarm_monitor_postpone_alarm(ALARM_MAX_RPM);
			tach_states_schedule_state(tach_states_get_past_state());
			break;
		case TACH_EVENT_REDRAW_SCREEN:
			display_timeout_user_active();
			
			if (settings_manager_get_beeper_enabled()) beeper_play_tone(200);
			
			if (ALARM_STATE_ARMED == alarm_get_alarm_state(ALARM_MAX_RPM))
			{
				tach_states_schedule_state(tach_states_get_past_state());
				break;
			}
			
			rpm = tach_monitor_get_rpm();
			max_rpm = settings_manager_get_max_rpm();
						
			snprintf(pData->out_buf,
			         DISPLAY_LINE_SIZE+1,
					 "%u (%u)",
					 rpm,
					 max_rpm);
			pData->out_buf[DISPLAY_LINE_SIZE] = 0;
			displayPrintLine(pData->alarm_max_rpm_str_tmp, pData->out_buf);
			break;
		default:
			break;
	}
}