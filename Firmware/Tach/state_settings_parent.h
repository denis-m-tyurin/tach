#ifndef TACH_STATE_SETTINGS_PARENT_H
#define TACH_STATE_SETTINGS_PARENT_H

void state_settings_parent_enter(void **pStateBuf);
void state_settings_parent_exit(void **pStateBuf);
void state_settings_parent_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif