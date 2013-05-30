#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "alarm_monitor.h"
#include "tach_monitor.h"
#include "power_monitor.h"
#include "settings_manager.h"

#define ALARM_POSTPONE_TIMEOUT_SEC 30

/* Local vars */
static uint8_t alarm_state[ALARM_MAX_ID];
static uint8_t alarm_transition_timeouts[ALARM_MAX_ID];

void alarm_monitor_init()
{
	uint8_t counter;
	
	for (counter=0;counter<ALARM_MAX_ID;counter++)
	{
		alarm_state[counter] = ALARM_STATE_ARMED;
		alarm_transition_timeouts[counter] = 0;
	}
}

uint8_t alarm_monitor_1sec_tick_check_alarm()
{
	uint8_t counter;

	/* Do not fire multiple alarms */
	for (counter=0;counter<ALARM_MAX_ID;counter++)
	{
		if (ALARM_STATE_FIRED == alarm_state[counter])
		{
			/* check if the alarm is no longer active */
			if (!alarm_is_active(counter))
			{
				alarm_state[counter] = ALARM_STATE_ARMED;
			}

			/* exit here anyway */
			return ALARM_MAX_ID;
		}
		
		/* check postponed alarms and decrease timeout */
		if (ALARM_STATE_POSTPONED == alarm_state[counter])
		{
			alarm_transition_timeouts[counter]--;
			if (0 == alarm_transition_timeouts[counter])
			{
				alarm_state[counter] = ALARM_STATE_ARMED;
			}
		}
	}
	
	/* There is no fired alarm, check alarms again */
	/* Do not fire multiple alarms */
	for (counter=0;counter<ALARM_MAX_ID;counter++)
	{
		if (ALARM_STATE_ARMED == alarm_state[counter])
		{
			if (alarm_is_active(counter))
			{
				alarm_state[counter] = ALARM_STATE_FIRED;
				return counter;
			}
		}		
	}
	
	/* Nothing happens */
	return ALARM_MAX_ID;
}

void alarm_monitor_postpone_alarm(ALARM_ID_T alarm_id)
{
	if (ALARM_STATE_FIRED == alarm_state[alarm_id])
	{
		alarm_state[alarm_id] = ALARM_STATE_POSTPONED;
		alarm_transition_timeouts[alarm_id] = ALARM_POSTPONE_TIMEOUT_SEC;
	}
}

bool alarm_is_active(ALARM_ID_T alarm_id)
{
	uint16_t rpm_voltage;
	
	switch (alarm_id)
	{
		case ALARM_MIN_RPM:
			rpm_voltage = tach_monitor_get_rpm();
			if (0 != rpm_voltage)
			{
				if (rpm_voltage < settings_manager_get_min_rpm())
				{
					return true;
				}
			}
			return false;
		case ALARM_MAX_RPM:
			rpm_voltage = tach_monitor_get_rpm();
			if (rpm_voltage > settings_manager_get_max_rpm())
			{
				return true;
			}			
			return false;
		case ALARM_MIN_VOLTAGE:
			rpm_voltage = power_monitor_get_voltage();
			if (rpm_voltage < settings_manager_get_min_voltage())
			{
				return true;
			}
			return false;			
		case ALARM_MAX_VOLTAGE:
			rpm_voltage = power_monitor_get_voltage();
			if (rpm_voltage > settings_manager_get_max_voltage())
			{
				return true;
			}
			return false;			
		default:
			return false;
	}
}

uint8_t alarm_get_alarm_state (ALARM_ID_T alarm_id)
{
	return alarm_state[alarm_id];
}