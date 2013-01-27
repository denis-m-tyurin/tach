#ifndef TACH_STATES_H
#define TACH_STATES_H

#include <avr/io.h>
#include <stddef.h>

/*	States description
 *
 *	< MAIN SCREEN (tach, voltage, temp)			>
 *				|
 *	< TACH ADVANCED (tach with pseudo-graph)	>
 *				|
 *	< ONBOARD TOP LIGHT SWITCH					>
 *				|
 *	< ONBOARD SIDE LIGHTS SWITCH				>
 *				|
 *	< SETTINGS									>
 *							|
 *	< SETTINGS:	SET MAX TACH					>
 *							|
 *	< SETTINGS:	SET MIN TACH					>
 *							|
 *	< SETTINGS:	SET BACKLIGHT INTENSITY			>
 *							|
 *	< SETTINGS:	SET BACKLIGHT TIMEOUT			> 
 *							|
 *	< SETTINGS:	SET VOLTAGE COMPENSATION		>
 *							|
 *	< SETTINGS:	SET AUTO OFF VOLTAGE			>
 *							|
 *	< SETTINGS:	EXIT							>
 *				|
 */

typedef enum 
{
	TACH_STATE_NO_STATE = -1,
	TACH_STATE_MAIN_SCREEN = 0,
	TACH_STATE_TOP_LIGHT_SWITCH,
	
	TACH_STATE_SETTINGS,
	
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
void tach_states_schedule_state(TACH_STATE_ID_T state_id);

TACH_STATE_ID_T tach_states_get_next_state();
TACH_STATE_ID_T tach_states_get_prev_state();
TACH_STATE_ID_T tach_states_get_scheduled_state();

void tach_states_dispatch_event(uint8_t event, void *data);
#endif