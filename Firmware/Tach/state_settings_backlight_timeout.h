#ifndef TACH_STATE_SETTINGS_BACKLIGHT_TIMEOUT_H
#define TACH_STATE_SETTINGS_BACKLIGHT_TIMEOUT_H

void state_settings_backlight_timeout_enter(void **pStateBuf);
void state_settings_backlight_timeout_exit(void **pStateBuf);
void state_settings_backlight_timeout_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif