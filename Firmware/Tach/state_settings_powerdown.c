#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "state_settings_powerdown.h"
#include "states.h"
#include "utils.h"
#include "display.h"


const char settings_powerdown_str1[] PROGMEM = "   ÂÛÊËÞ×ÈÒÜ?";
const char settings_powerdown_str2[] PROGMEM = " <םאזלט הגאזהû>";

typedef struct 
{
	char* settings_powerdown_str1_tmp;
	char* settings_powerdown_str2_tmp;
	uint8_t click_count;
} settings_powerdown_state_strings;

void state_settings_powerdown_enter(void **pStateBuf)
{
	settings_powerdown_state_strings *pData;
	displayClear();	
	*pStateBuf = malloc(sizeof(settings_powerdown_state_strings));
	pData = (settings_powerdown_state_strings*) *pStateBuf;
	pData->settings_powerdown_str1_tmp = utils_read_string_from_progmem(settings_powerdown_str1);
	pData->settings_powerdown_str2_tmp = utils_read_string_from_progmem(settings_powerdown_str2);
	pData->click_count = 0;	
}

void state_settings_powerdown_exit(void **pStateBuf)
{
		settings_powerdown_state_strings *pData = (settings_powerdown_state_strings*) *pStateBuf;
		
		if (NULL != pData->settings_powerdown_str1_tmp)
		{
			free(pData->settings_powerdown_str1_tmp);
		}
		if (NULL != pData->settings_powerdown_str2_tmp)
		{
			free(pData->settings_powerdown_str2_tmp);
		}		
		if (NULL != *pStateBuf)
		{
			free(*pStateBuf);
		}
}


void state_settings_powerdown_event_handler(uint8_t event, void **pStateBuf, void *data)
{	
	settings_powerdown_state_strings *pData = (settings_powerdown_state_strings*) *pStateBuf;

	switch (event)
	{
		case TACH_EVENT_ENCODER_BUTTON_PRESSED:
			pData->click_count++;
			
			if (2 == pData->click_count)
			{
				/* Completelly power down the device. Important thing: don't change 
				 * state of side and top lights as it might be still wanted to be enabled */
				cli();
				display_set_backlight(DISPLAY_BACKLIGHT_OFF);
				displayPrintLine("","");
				set_sleep_mode(SLEEP_MODE_PWR_DOWN);
				sleep_mode();
				/* never gets here */
			}			
		
			break;	
		case TACH_EVENT_REDRAW_SCREEN:
			displayPrintLine(pData->settings_powerdown_str1_tmp, pData->settings_powerdown_str2_tmp);
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			/* schedule next state in view mode */
			tach_states_schedule_state(tach_states_get_next_state());	
			break;
		case TACH_EVENT_ENCODER_LEFT:
			/* Jump into last settings state */
			tach_states_schedule_state(tach_states_get_prev_state());							
			break;		
		default:
			break;				
	}	
}