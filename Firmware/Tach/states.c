
#include "states.h"

/* include all the states */
#include "state_main_screen.h"
#include "state_voltage_advanced.h"
#include "state_top_light_switch.h"
#include "state_side_light_switch.h"
#include "state_settings_parent.h"
#include "state_settings_tach_pulses.h"
#include "state_settings_min_rpm.h"
#include "state_settings_max_rpm.h"
#include "state_settings_backlight_intensity.h"
#include "state_settings_backlight_timeout.h"
#include "state_settings_backlight_tach_on.h"
#include "state_settings_voltage_comp.h"
#include "state_settings_exit.h"
#include "state_settings_max_voltage.h"
#include "state_settings_min_voltage.h"
#include "state_alarm_min_rpm.h"
#include "state_alarm_max_rpm.h"
#include "state_alarm_min_voltage.h"
#include "state_alarm_max_voltage.h"
#include "state_settings_powerdown.h"
#include "state_settings_beeper_enabled.h"
#include "state_settings_min_voltage_alarm_on.h"
#include "state_settings_max_voltage_alarm_on.h"
#include "state_settings_min_rpm_alarm_on.h"
#include "state_settings_max_rpm_alarm_on.h"


static void *pCurrentStateBuf = NULL;

static TACH_STATE_ID_T s_current_state = TACH_STATE_NO_STATE;
static TACH_STATE_ID_T s_scheduled_state = TACH_STATE_NO_STATE;
static TACH_STATE_ID_T s_past_state = TACH_STATE_NO_STATE;

TACH_STATE_T s_states[] = {
		{TACH_STATE_NO_STATE,
		NULL,
		NULL,
		NULL},
		{TACH_STATE_MAIN_SCREEN,
		state_main_screen_state_enter,
		state_main_screen_state_exit,
		state_main_screen_state_event_handler},
		{TACH_STATE_VOLTAGE_ADVANCED,
		state_voltage_advanced_state_enter,
		state_voltage_advanced_state_exit,
		state_voltage_advanced_state_event_handler},
		{TACH_STATE_TOP_LIGHT_SWITCH,
		state_top_light_switch_enter,
		state_top_light_switch_exit,
		state_top_light_switch_event_handler},
		{TACH_STATE_SIDE_LIGHT_SWITCH,
		state_side_light_switch_enter,
		state_side_light_switch_exit,
		state_side_light_switch_event_handler},		
		{TACH_STATE_SETTINGS_PARENT,
		state_settings_parent_enter,
		state_settings_parent_exit,
		state_settings_parent_event_handler},		
		{TACH_STATE_SETTINGS_TACH_PULSES,
		state_settings_tach_pulses_enter,
		state_settings_tach_pulses_exit,
		state_settings_tach_pulses_event_handler},		
		{TACH_STATE_SETTINGS_MIN_RPM,
		state_settings_min_rpm_enter,
		state_settings_min_rpm_exit,
		state_settings_min_rpm_event_handler},
		{TACH_STATE_SETTINGS_MAX_RPM,
		state_settings_max_rpm_enter,
		state_settings_max_rpm_exit,
		state_settings_max_rpm_event_handler},		
		{TACH_STATE_SETTINGS_BACKLIGHT_INTENSITY,
		state_settings_backlight_intensity_enter,
		state_settings_backlight_intensity_exit,
		state_settings_backlight_intensity_event_handler},		
		{TACH_STATE_SETTINGS_BACKLIGHT_TIMEOUT,
		state_settings_backlight_timeout_enter,
		state_settings_backlight_timeout_exit,
		state_settings_backlight_timeout_event_handler},
		{TACH_STATE_SETTINGS_BACKLIGHT_TACH_ON,
		state_settings_backlight_tach_on_enter,
		state_settings_backlight_tach_on_exit,
		state_settings_backlight_tach_on_event_handler},		
		{TACH_STATE_SETTINGS_MIN_VOLTAGE,
		state_settings_min_voltage_enter,
		state_settings_min_voltage_exit,
		state_settings_min_voltage_event_handler},
		{TACH_STATE_SETTINGS_MAX_VOLTAGE,
		state_settings_max_voltage_enter,
		state_settings_max_voltage_exit,
		state_settings_max_voltage_event_handler},		
		{TACH_STATE_SETTINGS_VOLTAGE_COMP,
		state_settings_voltage_comp_enter,
		state_settings_voltage_comp_exit,
		state_settings_voltage_comp_event_handler},		
		{TACH_STATE_SETTINGS_BEEPER_ENABLED,
		state_settings_beeper_enabled_enter,
		state_settings_beeper_enabled_exit,
		state_settings_beeper_enabled_event_handler},
		{TACH_STATE_SETTINGS_MIN_VOLTAGE_ALARM_ON,
		state_settings_min_voltage_alarm_on_enter,
		state_settings_min_voltage_alarm_on_exit,
		state_settings_min_voltage_alarm_on_event_handler},
		{TACH_STATE_SETTINGS_MAX_VOLTAGE_ALARM_ON,
		state_settings_max_voltage_alarm_on_enter,
		state_settings_max_voltage_alarm_on_exit,
		state_settings_max_voltage_alarm_on_event_handler},
		{TACH_STATE_SETTINGS_MIN_RPM_ALARM_ON,
		state_settings_min_rpm_alarm_on_enter,
		state_settings_min_rpm_alarm_on_exit,
		state_settings_min_rpm_alarm_on_event_handler},
		{TACH_STATE_SETTINGS_MAX_RPM_ALARM_ON,
		state_settings_max_rpm_alarm_on_enter,
		state_settings_max_rpm_alarm_on_exit,
		state_settings_max_rpm_alarm_on_event_handler},
		{TACH_STATE_SETTINGS_POWERDOWN,
		state_settings_powerdown_enter,
		state_settings_powerdown_exit,
		state_settings_powerdown_event_handler},
		{TACH_STATE_SETTINGS_EXIT,
		state_settings_exit_enter,
		state_settings_exit_exit,
		state_settings_exit_event_handler},		
		{TACH_STATE_ALARM_MIN_RPM,
		state_alarm_min_rpm_enter,
		state_alarm_min_rpm_exit,
		state_alarm_min_rpm_event_handler},		
		{TACH_STATE_ALARM_MAX_RPM,
		state_alarm_max_rpm_enter,
		state_alarm_max_rpm_exit,
		state_alarm_max_rpm_event_handler},
		{TACH_STATE_ALARM_MIN_VOLTAGE,
		state_alarm_min_voltage_enter,
		state_alarm_min_voltage_exit,
		state_alarm_min_voltage_event_handler},
		{TACH_STATE_ALARM_MAX_VOLTAGE,
		state_alarm_max_voltage_enter,
		state_alarm_max_voltage_exit,
		state_alarm_max_voltage_event_handler},
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
			s_states[s_current_state].state_exit(&pCurrentStateBuf);
		}
	
		/* Init the new state */
		s_past_state = s_current_state;
		s_current_state = state_id;
		s_states[s_current_state].state_enter(&pCurrentStateBuf);
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
		prev_state = TACH_STATE_STATE_MAX - 1;
	}
	
	return prev_state;
}

TACH_STATE_ID_T tach_states_get_scheduled_state()
{
	TACH_STATE_ID_T scheduled_state = s_scheduled_state;
	s_scheduled_state = TACH_STATE_NO_STATE;
	return scheduled_state;
}


TACH_STATE_ID_T tach_states_get_past_state()
{
	return s_past_state;
}

void tach_states_dispatch_event(uint8_t event, void *data)
{
	/* Check if any state is set */
	if (s_current_state == TACH_STATE_NO_STATE) 
		return;
	
	/* Dispatch event to current state */
	s_states[s_current_state].state_event_handler(event, &pCurrentStateBuf, data);
	
}
