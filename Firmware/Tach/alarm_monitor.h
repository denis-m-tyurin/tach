#ifndef ALARM_MONITOR_H
#define ALARM_MONITOR_H

#include <stdbool.h>

typedef enum 
{
	ALARM_MIN_RPM = 0,
	ALARM_MAX_RPM,
	ALARM_MIN_VOLTAGE,	
	ALARM_MAX_VOLTAGE,
	
	ALARM_MAX_ID	
} ALARM_ID_T;

#define ALARM_STATE_ARMED 0
#define ALARM_STATE_FIRED 1
#define ALARM_STATE_POSTPONED 2

void alarm_monitor_init();
ALARM_ID_T alarm_monitor_1sec_tick_check_alarm();
void alarm_monitor_postpone_alarm(ALARM_ID_T alarm_id);
uint8_t alarm_get_alarm_state (ALARM_ID_T alarm_id);

bool alarm_is_active(ALARM_ID_T alarm_id);

#endif