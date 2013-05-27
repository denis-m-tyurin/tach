#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "temp_monitor.h"

static one_wire_bus_data_t* pBus = NULL;

static volatile uint8_t conversion_state = TEMP_MONITOR_CONVERSION_IDLE;

void temp_monitor_init()
{
	/* Initialize 1-wire bus */
	pBus = 	one_wire_initialize_bus(ONE_WIRE_PORT_A, PA4);
	
	if (NULL != pBus)
	{
		if (one_wire_check_presence(pBus))
		{
			/* Good news - the sensor is here */
			conversion_state = TEMP_MONITOR_CONVERSION_IDLE;
		}
	}
}

void temp_monitor_start_conversion()
{
	if ((conversion_state == TEMP_MONITOR_CONVERSION_IDLE))
	{
		if (one_wire_check_presence(pBus))
		{
			/* Send SKIP ROM [CCh] */
			one_wire_send_command(pBus, 0xCC);
		
			/* Send CONVERT T [44h] */
			one_wire_send_command(pBus, 0x44);
		
			conversion_state = TEMP_MONITOR_CONVERSION_IN_PROGRESS;
		}
	}
}

uint8_t temp_monitor_get_conversion_status()
{
	if (TEMP_MONITOR_CONVERSION_IN_PROGRESS == conversion_state)
	{
		/* Check if conversion finished */
		if (true == one_wire_is_conversion_done(pBus))
		{
			conversion_state = TEMP_MONITOR_CONVERSION_DONE;
		}
	}

	return conversion_state;
}

bool temp_monitor_get_temperature(one_wire_temperature_data_t *pTempData)
{
	uint16_t sensor_data = 0;
	
	if ((TEMP_MONITOR_CONVERSION_DONE != conversion_state) || (NULL == pTempData))
	{
		return false;
	}
	
	if (!one_wire_check_presence(pBus))
	{
		return false;
	}

	/* Read temp from the sensor */
	
	/* Send SKIP ROM [CCh] */
	one_wire_send_command(pBus, 0xCC);

	/* Send READ SCRATCHPAD [BEh] */
	one_wire_send_command(pBus, 0xBE);

	/* Read data */
	sensor_data = one_wire_receive_data(pBus);
	
	one_wire_temperature_convert(pTempData, sensor_data);
	
	conversion_state = TEMP_MONITOR_CONVERSION_IDLE;
	
	return true;
}
