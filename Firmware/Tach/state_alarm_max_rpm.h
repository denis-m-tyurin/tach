#ifndef STATE_ALARM_MAX_RPM_H
#define STATE_ALARM_MAX_RPM_H

void state_alarm_max_rpm_enter(void **pStateBuf);
void state_alarm_max_rpm_exit(void **pStateBuf);
void state_alarm_max_rpm_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif