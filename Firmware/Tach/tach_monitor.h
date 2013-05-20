#ifndef TACH_MONITOR_H
#define TACH_MONITOR_H

#define TACH_MONITOR_PULSES_TO_PROBE 10
#define ONE_TICK_TIME_MS 4
#define ONE_MINUTE_IN_MS 60000000 

void tach_monitor_init(uint8_t pulses_per_revolution);
void tach_monitor_update_rpm(uint32_t ticks);
uint16_t tach_monitor_get_rpm();
void tach_monitor_reset_rpm();

#endif