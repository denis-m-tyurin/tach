#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

void power_monitor_init();
uint16_t power_monitor_get_voltage();
void power_monitor_set_voltage_compensation(uint8_t compensation);

#endif