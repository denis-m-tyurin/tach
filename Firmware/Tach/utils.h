#ifndef TACH_UTILS_H
#define TACH_UTILS_H

#include <avr/pgmspace.h>

/* Don't forget to release memory allocated by this 
   func whenever it returns not NULL */
char* utils_read_string_from_progmem(char* str_to_read);

#endif