#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdio.h>

/* Depends on ICR1 which is used for beeper, therefore is adjusted for specific freq */
#define BACKLIGHT_TOP 70
#define DISPLAY_BACKLIGHT_OFF 0

void display_set_backlight(uint8_t backlight_level);
void initDisplay();
void displayClear();
void displayPrintString(const char *string);
void displayPrintLine(const char *line1, const char* line2);

#endif