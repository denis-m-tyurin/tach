#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdio.h>

#define DISPLAY_BACKLIGHT_ON 100
#define DISPLAY_BACKLIGHT_OFF 0

void display_set_backlight(uint8_t percent);
void initDisplay();
void displayClear();
void displayPrintString(const char *string);
void displayPrintLine(const char *line1, const char* line2);

#endif