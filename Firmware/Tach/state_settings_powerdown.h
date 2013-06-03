#ifndef TACH_STATE_SETTINGS_POWERDOWN_H
#define TACH_STATE_SETTINGS_POWERDOWN_H

void state_settings_powerdown_enter(void **pStateBuf);
void state_settings_powerdown_exit(void **pStateBuf);
void state_settings_powerdown_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif