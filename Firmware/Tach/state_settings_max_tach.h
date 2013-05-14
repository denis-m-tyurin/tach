#ifndef TACH_STATE_SETTINGS_MAX_TACH_H
#define TACH_STATE_SETTINGS_MAX_TACH_H

void state_settings_max_tach_enter(void **pStateBuf);
void state_settings_max_tach_exit(void **pStateBuf);
void state_settings_max_tach_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif