#ifndef STATE_ALARM_MIN_VOLTAGE_H
#define STATE_ALARM_MIN_VOLTAGE_H

void state_alarm_min_voltage_enter(void **pStateBuf);
void state_alarm_min_voltage_exit(void **pStateBuf);
void state_alarm_min_voltage_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif