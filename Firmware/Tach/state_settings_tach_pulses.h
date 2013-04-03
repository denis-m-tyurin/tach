#ifndef TACH_STATE_SETTINGS_TACH_PULSES_H
#define TACH_STATE_SETTINGS_TACH_PULSES_H

void state_settings_tach_pulses_enter(void **pStateBuf);
void state_settings_tach_pulses_exit(void **pStateBuf);
void state_settings_tach_pulses_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif