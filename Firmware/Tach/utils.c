#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

char* utils_read_string_from_progmem(const char* str_to_read)
{
	size_t length = strlen_P(str_to_read);
	char* result = NULL;
	
	if (length > 0)	
	{
		result = malloc((length + 1 ) * sizeof(char));
		strcpy_P(result, str_to_read);
	}
	
	return result;
}