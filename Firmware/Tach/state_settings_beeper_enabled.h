#ifndef TACH_STATE_SETTINGS_BEEPER_ENABLED_H
#define TACH_STATE_SETTINGS_BEEPER_ENABLED_H

void state_settings_beeper_enabled_enter(void **pStateBuf);
void state_settings_beeper_enabled_exit(void **pStateBuf);
void state_settings_beeper_enabled_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif