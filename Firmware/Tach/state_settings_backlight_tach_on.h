#ifndef TACH_STATE_SETTINGS_BACKLIGHT_TACH_ON_H
#define TACH_STATE_SETTINGS_BACKLIGHT_TACH_ON_H

void state_settings_backlight_tach_on_enter(void **pStateBuf);
void state_settings_backlight_tach_on_exit(void **pStateBuf);
void state_settings_backlight_tach_on_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif