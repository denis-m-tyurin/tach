#ifndef TACH_STATE_SETTINGS_BACKLIGHT_INTENSITY_H
#define TACH_STATE_SETTINGS_BACKLIGHT_INTENSITY_H

void state_settings_backlight_intensity_enter(void **pStateBuf);
void state_settings_backlight_intensity_exit(void **pStateBuf);
void state_settings_backlight_intensity_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif