#ifndef TACH_STATE_VOLTAGE_ADVANCED_H
#define TACH_STATE_VOLTAGE_ADVANCED_H

void state_voltage_advanced_state_enter(void **pStateBuf);
void state_voltage_advanced_state_exit(void **pStateBuf);
void state_voltage_advanced_state_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif