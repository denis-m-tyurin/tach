#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "state_main_screen.h"
#include "states.h"
#include "power_monitor.h"

void state_main_screen_state_enter(void *pStateBuf)
{
	
}

void state_main_screen_state_exit(void *pStateBuf)
{
	
}


void state_main_screen_state_event_handler(uint8_t event, void *pStateBuf)
{	
	char *out_buf = NULL;
	uint16_t voltage = 0;
	
	switch (event)
	{
		case TACH_EVENT_REDRAW_SCREEN:
			voltage = power_monitor_get_voltage();
			out_buf = (char*) malloc(17);
			snprintf(out_buf, 16, "%.2u.%.2uV                 ", voltage/66, voltage % 66);
			displayPrintLine("REdRAW", out_buf);
			free(out_buf);
			break;
		case TACH_EVENT_ENCODER_RIGHT:
			displayPrintLine("ENCODER                 ", "RIGHT                 ");
			break;
		case TACH_EVENT_ENCODER_LEFT:
			displayPrintLine("ENCODER                 ", "LEFT                 ");
			break;			
		default:
			break;				
	}	
}