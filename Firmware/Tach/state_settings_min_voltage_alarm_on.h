#ifndef TACH_STATE_SETTINGS_MIN_VOLTAGE_ALARM_ON_H
#define TACH_STATE_SETTINGS_MIN_VOLTAGE_ALARM_ON_H

void state_settings_min_voltage_alarm_on_enter(void **pStateBuf);
void state_settings_min_voltage_alarm_on_exit(void **pStateBuf);
void state_settings_min_voltage_alarm_on_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif