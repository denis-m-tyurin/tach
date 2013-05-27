#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdio.h>

/* Depends on ICR1 which is used for beeper, therefore is adjusted for specific freq */
#define DISPLAY_BACKLIGHT_TOP 70
#define DISPLAY_BACKLIGHT_OFF 0

/* min backlight timeout value, sec */
#define DISPLAY_BACKLIGHT_TIMEOUT_MIN_SEC 5 

/* max backlight timeout value, this means backlight is always ON */
#define DISPLAY_BACKLIGHT_TIMEOUT_ALWAYS_ON 180 

void display_set_backlight(uint8_t backlight_level);
void initDisplay();
void displayClear();
void displayPrintString(const char *string);
void displayPrintLine(const char *line1, const char* line2);

void display_set_timeout(uint16_t display_timeout);
void display_timeout_user_active();
void display_timeout_1sec_tick();

#endif