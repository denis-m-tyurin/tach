#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "tach_monitor.h"

static volatile uint32_t minute_equivalent = 0;
static volatile uint16_t rpm = 0;

void tach_monitor_init(uint8_t pulses_per_revolution)
{
	minute_equivalent = ONE_MINUTE_IN_MS / pulses_per_revolution;	
}

void tach_monitor_update_rpm(uint32_t ticks)
{
	rpm = minute_equivalent / ((ticks * ONE_TICK_TIME_MS) / TACH_MONITOR_PULSES_TO_PROBE);
}

uint16_t tach_monitor_get_rpm()
{
	return rpm;
}

void tach_monitor_reset_rpm()
{
	rpm = 0;
}