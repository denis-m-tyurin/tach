#ifndef ENCODER_MONITOR_H
#define ENCODER_MONITOR_H

typedef enum 
{
	ENCODER_ACTION_NO_ACTION = -1,
	ENCODER_ACTION_RIGHT = 0,
	ENCODER_ACTION_LEFT,	
	
	ENCODER_ACTION_MAX
} ENCODER_ACTION_ID_T;

void encoder_monitor_init();
void encoder_monitor_handle_timer_int();
uint8_t encoder_monitor_get_last_action();

#endif