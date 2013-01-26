#ifndef TACH_STATES_H
#define TACH_STATES_H

#include <avr/io.h>
#include <stddef.h>

typedef enum 
{
	TACH_STATE_NO_STATE = -1,
	TACH_STATE_MAIN_SCREEN = 0,
	TACH_STATE_SETTINGS_MAIN,	
	
	TACH_STATE_STATE_MAX
} TACH_STATE_ID_T;

typedef enum 
{
	TACH_EVENT_REDRAW_SCREEN = 0,
	TACH_EVENT_ENCODER_RIGHT,
	TACH_EVENT_ENCODER_LEFT,
		
	TACH_EVENT_MAX
};

typedef struct  
{
	int state_id;
	void (*state_enter)(void *);
	void (*state_exit)(void *);
	void (*state_event_handler)(uint8_t, void *);	
} TACH_STATE_T;

void tach_states_set_state(TACH_STATE_ID_T state_id);
void tach_states_go_to_next_state();
void tach_states_go_to_prev_state();
void tach_states_dispatch_event(uint8_t event, void *data);
#endif