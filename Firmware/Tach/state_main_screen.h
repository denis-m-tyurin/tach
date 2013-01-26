#ifndef TACH_STATE_MAIN_SCREEN_H
#define TACH_STATE_MAIN_SCREEN_H

void state_main_screen_state_enter(void *pStateBuf);
void state_main_screen_state_exit(void *pStateBuf);
void state_main_screen_state_event_handler(uint8_t event, void *pStateBuf);
	
#endif