#ifndef TACH_STATE_TACH_ADVANCED_H
#define TACH_STATE_TACH_ADVANCED_H

void state_tach_advanced_state_enter(void **pStateBuf);
void state_tach_advanced_state_exit(void **pStateBuf);
void state_tach_advanced_state_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif