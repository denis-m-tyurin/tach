#ifndef TACH_STATE_SIDE_LIGHT_SWITCH_H
#define TACH_STATE_SIDE_LIGHT_SWITCH_H

void state_side_light_switch_enter(void **pStateBuf);
void state_side_light_switch_exit(void **pStateBuf);
void state_side_light_switch_event_handler(uint8_t event, void **pStateBuf, void *data);
	
#endif