#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "tach_monitor.h"

#define TACH_MONITOR_PEAK_FILTER_LIMIT 1000 

#define TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK 0
#define TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_BELOW 1
#define TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_ABOVE 2

static volatile uint32_t minute_equivalent = 0;
static volatile uint16_t rpm = 0;
static volatile uint8_t peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK;

void tach_monitor_init(uint8_t pulses_per_revolution)
{
	minute_equivalent = ONE_MINUTE_IN_MS / pulses_per_revolution;	
}

void tach_monitor_update_rpm(uint32_t ticks)
{
	uint32_t tmp_rpm = minute_equivalent / ((ticks * ONE_TICK_TIME_MS) / TACH_MONITOR_PULSES_TO_PROBE);
	
	if (tmp_rpm > rpm)
	{
		if (tmp_rpm > (rpm + TACH_MONITOR_PEAK_FILTER_LIMIT))
		{
			/* Peak detected */
			if (TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_ABOVE == peak_filtering_state)
			{
				/* Not an error, just update rpm */
				rpm = tmp_rpm;
				peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK;
			} 
			else
			{
				/* Don't update rpm, but wait for next measurement */
				peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_ABOVE;
			}
		}
		else 
		{
			rpm = tmp_rpm;
			peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK;
		}
	} else if (tmp_rpm < rpm)
	{
		if (tmp_rpm < (rpm - TACH_MONITOR_PEAK_FILTER_LIMIT))
		{
			/* Peak detected */
			if (TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_BELOW == peak_filtering_state)
			{
				/* Not an error, just update rpm */
				rpm = tmp_rpm;
				peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK;
			}
			else
			{
				/* Don't update rpm, but wait for next measurement */
				peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_PEAK_BELOW;
			}
		}
		else
		{
			rpm = tmp_rpm;
			peak_filtering_state = TACH_MONITOR_PEAK_FILTERING_STATE_NO_PEAK;
		}		
	}
}

uint16_t tach_monitor_get_rpm()
{
	return rpm;
}

void tach_monitor_reset_rpm()
{
	rpm = 0;
}