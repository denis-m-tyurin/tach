#ifndef TACH_STATE_SETTINGS_EXIT_H
#define TACH_STATE_SETTINGS_EXIT_H

void state_settings_exit_enter(void **pStateBuf);
void state_settings_exit_exit(void **pStateBuf);
void state_settings_exit_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif