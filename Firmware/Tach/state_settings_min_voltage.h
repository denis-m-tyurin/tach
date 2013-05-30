#ifndef TACH_STATE_SETTINGS_MIN_VOLTAGE_H
#define TACH_STATE_SETTINGS_MIN_VOLTAGE_H

void state_settings_min_voltage_enter(void **pStateBuf);
void state_settings_min_voltage_exit(void **pStateBuf);
void state_settings_min_voltage_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif