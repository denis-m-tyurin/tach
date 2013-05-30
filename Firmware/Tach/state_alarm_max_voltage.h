#ifndef STATE_ALARM_MAX_VOLTAGE_H
#define STATE_ALARM_MAX_VOLTAGE_H

void state_alarm_max_voltage_enter(void **pStateBuf);
void state_alarm_max_voltage_exit(void **pStateBuf);
void state_alarm_max_voltage_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif