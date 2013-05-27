#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H
#include "one_wire.h"

void temp_monitor_init();
void temp_monitor_start_conversion();

#define TEMP_MONITOR_CONVERSION_IDLE 0
#define TEMP_MONITOR_CONVERSION_IN_PROGRESS 1
#define TEMP_MONITOR_CONVERSION_DONE 2

uint8_t temp_monitor_get_conversion_status();

bool temp_monitor_get_temperature(one_wire_temperature_data_t *pTempData);

#endif