#ifndef TACH_STATE_TOP_LIGHT_SWITCH_H
#define TACH_STATE_TOP_LIGHT_SWITCH_H

void state_top_light_switch_enter(void *pStateBuf);
void state_top_light_switch_exit(void *pStateBuf);
void state_top_light_switch_event_handler(uint8_t event, void *pStateBuf);
	
#endif