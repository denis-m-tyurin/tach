
#include "states.h"

/* include all the states */
#include "state_main_screen.h"
#include "state_top_light_switch.h"

static void *pCurrentStateBuf = NULL;

static TACH_STATE_ID_T s_current_state = TACH_STATE_NO_STATE;
static TACH_STATE_ID_T s_scheduled_state = TACH_STATE_NO_STATE;

TACH_STATE_T s_states[] = {
		{TACH_STATE_MAIN_SCREEN,
		state_main_screen_state_enter,
		state_main_screen_state_exit,
		state_main_screen_state_event_handler},
		{TACH_STATE_TOP_LIGHT_SWITCH,
		state_top_light_switch_enter,
		state_top_light_switch_exit,
		state_top_light_switch_event_handler},
};

void tach_states_set_state(TACH_STATE_ID_T state_id)
{
	/* Check if appropriate state ID exists */
	if ((state_id <= TACH_STATE_NO_STATE) || (state_id >= TACH_STATE_STATE_MAX))
		return;
	
	/* If it is the same state - no need to exit and then re-init */
	if (state_id != s_current_state)
	{
		/* Check whether we have an active state. Tear it down if necessary */
		if (TACH_STATE_NO_STATE != s_current_state)
		{
			s_states[s_current_state].state_exit(pCurrentStateBuf);
		}
	
		/* Init the new state */
		s_current_state = state_id;
		s_states[s_current_state].state_enter(pCurrentStateBuf);
	}
}

void tach_states_schedule_state(TACH_STATE_ID_T state_id)
{
	s_scheduled_state = state_id;
}

TACH_STATE_ID_T tach_states_get_next_state()
{
	TACH_STATE_ID_T next_state = s_current_state + 1;
	
	if (TACH_STATE_STATE_MAX == next_state)
	{
		/* Start from the beginning */
		next_state = TACH_STATE_NO_STATE + 1;
	}
	
	return next_state;
}

TACH_STATE_ID_T tach_states_get_prev_state()
{
	TACH_STATE_ID_T prev_state = s_current_state - 1;
	
	if (TACH_STATE_NO_STATE == prev_state)
	{
		/* Jump to the last state  */
		prev_state = TACH_EVENT_MAX - 1;
	}
	
	return prev_state;
}

TACH_STATE_ID_T tach_states_get_scheduled_state()
{
	TACH_STATE_ID_T scheduled_state = s_scheduled_state;
	s_scheduled_state = TACH_STATE_NO_STATE;
	return scheduled_state;
}

void tach_states_dispatch_event(uint8_t event, void *data)
{
	/* Check if any state is set */
	if (s_current_state == TACH_STATE_NO_STATE) 
		return;
	
	/* Dispatch event to current state */
	s_states[s_current_state].state_event_handler(event, data);
	
}