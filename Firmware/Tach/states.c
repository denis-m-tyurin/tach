
#include "states.h"

/* include all the states */
#include "state_main_screen.h"

static void *pCurrentStateBuf = NULL;

static TACH_STATE_ID_T s_current_state = TACH_STATE_NO_STATE;

TACH_STATE_T s_states[] = {
		{TACH_STATE_MAIN_SCREEN,
		state_main_screen_state_enter,
		state_main_screen_state_exit,
		state_main_screen_state_event_handler},			
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

void tach_states_go_to_next_state()
{
	
	
}
	
void tach_states_go_to_prev_state()
{
	
}

void tach_states_dispatch_event(uint8_t event, void *data)
{
	/* Check if any state is set */
	if (s_current_state == TACH_STATE_NO_STATE) 
		return;
	
	/* Dispatch event to current state */
	s_states[s_current_state].state_event_handler(event, data);
	
}
