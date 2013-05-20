#ifndef TACH_STATE_SETTINGS_MAX_RPM_H
#define TACH_STATE_SETTINGS_MAX_RPM_H

void state_settings_max_rpm_enter(void **pStateBuf);
void state_settings_max_rpm_exit(void **pStateBuf);
void state_settings_max_rpm_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif