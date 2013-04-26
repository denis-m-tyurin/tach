#ifndef TACH_STATE_SETTINGS_VOLTAGE_COMP_H
#define TACH_STATE_SETTINGS_VOLTAGE_COMP_H

void state_settings_voltage_comp_enter(void **pStateBuf);
void state_settings_voltage_comp_exit(void **pStateBuf);
void state_settings_voltage_comp_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif