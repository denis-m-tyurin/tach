
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include "display.h"
#include <avr/io.h>
#include <util/delay.h>

/********************************************
 *      PIN CONFIGURATION                   *
 *******************************************/
#define DISPLAY_BACKLIGHT_DDR DDRD
#define DISPLAY_BACKLIGHT_PIN PD5
#define DISPLAY_BACKLIGHT_PORT PORTD
#define DISPLAY_CONFIG_PINS_DDR DDRC
#define DISPLAY_CONFIG_PINS_PORT PORTC
#define DISPLAY_CONFIG_PIN_RS PC7
#define DISPLAY_CONFIG_PIN_E PC6
#define DISPLAY_DATA_PINS_DDR DDRC
#define DISPLAY_DATA_PINS_PORT PORTC
#define DISPLAY_DATA_PINS_MASK 0x3c /* Which pins are assigned for data lines */
#define DISPLAY_DATA_PINS_SHIFT 2 /* how much bits data should be shift in a port */
#define DISPLAY_DATA_PINS_INVERT 1 /* whether we need to invert data (negative order of data lines = 1, positive = 0) */

/********************************************
 *      COMMON DEFINES                      *
 *******************************************/

#undef	 DISPLAY_MODE_8BIT
#define	 DISPLAY_MODE_4BIT

#define DISPLAY_COMMAND_CLEAR_DISPLAY 		1
#define DISPLAY_COMMAND_RETURN_HOME 		1<<1
#define DISPLAY_COMMAND_ENTRY_MODE_SET		1<<2
#define DISPLAY_COMMAND_DISPLAY_ON_OFF		1<<3
#define DISPLAY_COMMAND_CURSOR_DISP_SHIFT	1<<4
#define DISPLAY_COMMAND_FUNCTION_SET		1<<5
#define DISPLAY_COMMAND_SET_DDRAM_ADDRESS	1<<7

#define DISPLAY_ENTRY_MODE_DECREMENT	0x0
#define DISPLAY_ENTRY_MODE_INCREMENT	0x2
#define DISPLAY_ENTRY_MODE_SHIFT_ON		0x1
#define DISPLAY_ENTRY_MODE_SHIFT_OFF	0x0
#define DISPLAY_ON						0x4
#define DISPLAY_OFF						0x0
#define DISPLAY_CURSOR_ON				0x2
#define DISPLAY_CURSOR_OFF				0x0
#define DISPLAY_CURSOR_BLINKING_ON		0x1
#define DISPLAY_CURSOR_BLINKING_OFF		0x0
#define DISPLAY_SHIFT_CURSOR			0x0
#define DISPLAY_SHIFT_DISPLAY			0x8
#define DISPLAY_SHIFT_LEFT				0x0
#define DISLAY_SHIFT_RIGHT				0x4
#define DISPLAY_DATA_LENGTH_8BIT		0x10
#define DISPLAY_DATA_LENGTH_4BIT		0x0
#define DISPLAY_TYPE_2LINE				0x8
#define DISPLAY_TYPE_1LINE				0x0
#define DISPLAY_SYMB_SIZE_5X11			0x4
#define DISPLAY_SYMB_SIZE_5X8			0x0


void displaySendCommand(uint8_t command, double delay_in_us);
void displaySendData(uint8_t data);
char displayTranslateSymb(const char symb);
static inline void delay_ms(uint16_t count);
static inline void delay_us(uint16_t count);

/********************************************
 *      PUBLIC FUNCTIONS                    *
 *******************************************/
void display_set_backlight( uint8_t percent )
{
	/* Configure pin just in case */
	DISPLAY_BACKLIGHT_DDR |= (1 << DISPLAY_BACKLIGHT_PIN);
	
	if (0 == percent)
	{
		/* Turn PWM off and set pin to 0 */
		DISPLAY_BACKLIGHT_PORT &= ~(1 << DISPLAY_BACKLIGHT_PIN);
		
	} else if ( 100 <= percent )
	{
		/* Turn PWM off and set pin to 1 */
		DISPLAY_BACKLIGHT_PORT |= (1 << DISPLAY_BACKLIGHT_PIN);
	} else
	{
		/* Enable PWM and set necessary pulse width */
		// TODO: implement
	}
	
	return;	
}

void initDisplay()
{
	uint8_t buf = 0;
	
	/*	  
	Start up steps 

	1. Wait for more than 40 ms after VDD rises to 4.5 V */

	DISPLAY_CONFIG_PINS_DDR  |= (1 << DISPLAY_CONFIG_PIN_RS) | (1 << DISPLAY_CONFIG_PIN_E);
	DISPLAY_CONFIG_PINS_PORT  &= ~(1 << DISPLAY_CONFIG_PIN_RS);
	DISPLAY_CONFIG_PINS_PORT  &= ~(1 << DISPLAY_CONFIG_PIN_E);

	DISPLAY_DATA_PINS_DDR |= DISPLAY_DATA_PINS_MASK;
	delay_ms(50);

	/* 	Wait for more than 39us */	
#ifdef DISPLAY_MODE_8BIT	
	displaySendCommand(DISPLAY_COMMAND_FUNCTION_SET | DISPLAY_DATA_LENGTH_8BIT | DISPLAY_TYPE_2LINE | DISPLAY_SYMB_SIZE_5X8,40);
#elif defined DISPLAY_MODE_4BIT
	displaySendCommand(DISPLAY_COMMAND_FUNCTION_SET | DISPLAY_DATA_LENGTH_4BIT, 40);
	
	if (0 == DISPLAY_DATA_PINS_INVERT )
	{
		/* No need to invert data in the port */
		// TODO: implement		
	} else
	{		
		buf = ((DISPLAY_COMMAND_FUNCTION_SET | DISPLAY_DATA_LENGTH_8BIT) >> 4);
		DISPLAY_DATA_PINS_PORT = ((((buf & 0x1) << 3) | ((buf & 0x2) << 1) | ((buf & 0x4) >> 1) | ((buf & 0x8) >> 3)) << DISPLAY_DATA_PINS_SHIFT) | (DISPLAY_DATA_PINS_PORT & (~DISPLAY_DATA_PINS_MASK));
	}
	
	
	/* Pull up E */
	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E);
	delay_us(40);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);

	displaySendCommand(DISPLAY_COMMAND_FUNCTION_SET | DISPLAY_DATA_LENGTH_4BIT | DISPLAY_TYPE_2LINE | DISPLAY_SYMB_SIZE_5X8,40);	
#else
	#error
#endif

	/* Display ON/OFF control */
	/* Wait for more than 37 us */
	displaySendCommand(DISPLAY_COMMAND_DISPLAY_ON_OFF | DISPLAY_ON | DISPLAY_CURSOR_OFF | DISPLAY_CURSOR_BLINKING_OFF,38);

	/* Clear display */

	/* Wait for more than 1.53 ms */
	displaySendCommand(DISPLAY_COMMAND_CLEAR_DISPLAY, 1540);

	/* Entry mode set */
	/* Wait for more than 39 us */

	displaySendCommand(DISPLAY_COMMAND_ENTRY_MODE_SET | DISPLAY_ENTRY_MODE_INCREMENT | DISPLAY_ENTRY_MODE_SHIFT_OFF, 40);

	/* Move cursor */
	displaySendCommand(DISPLAY_COMMAND_RETURN_HOME, 1540);

}

void displayClear()
{
	/* Clear display */
	/* Wait for more than 1.53 ms */
	displaySendCommand(DISPLAY_COMMAND_CLEAR_DISPLAY, 1540);

	/* Why don't just set DDRAM address? It takes less time */
	displaySendCommand(DISPLAY_COMMAND_RETURN_HOME, 1540);
}

void displayPrintString(const char *string)
{
	uint16_t counter = 0;
	while (string[counter] != 0)
	{
		displaySendData(displayTranslateSymb(string[counter]));
		counter++;
	}
}

void displayPrintLine(const char *line1, const char* line2)
{
	//displaySendCommand(DISPLAY_COMMAND_RETURN_HOME, 1540);
	displaySendCommand(DISPLAY_COMMAND_SET_DDRAM_ADDRESS, 40);
	displayPrintString(line1);


	/* Shift DDRAM address pointer to 40h */
	displaySendCommand(DISPLAY_COMMAND_SET_DDRAM_ADDRESS | 0x40, 40);
	displayPrintString(line2);
}

/********************************************
 *      LOCAL FUNCTIONS                     *
 *******************************************/

void displaySendCommand(uint8_t command, double delay_in_us)
{
	uint8_t buf = 0;
	DISPLAY_CONFIG_PINS_PORT  &= ~(1 << DISPLAY_CONFIG_PIN_RS ); // 0 - command, 1 - data 
	
#ifdef DISPLAY_MODE_8BIT	

	DISPLAY_DATA_PINS_PORT = command;	

	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E );
	delay_us(delay_us);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);

#elif defined DISPLAY_MODE_4BIT

	if (0 == DISPLAY_DATA_PINS_INVERT )
	{
		/* No need to invert data in the port */
		// TODO: implement		
	} else
	{			
		buf = (command & 0xF0) >> 4;
		DISPLAY_DATA_PINS_PORT = ((((buf & 0x1) << 3) | ((buf & 0x2) << 1) | ((buf & 0x4) >> 1) | ((buf & 0x8) >> 3)) << DISPLAY_DATA_PINS_SHIFT) | (DISPLAY_DATA_PINS_PORT & (~DISPLAY_DATA_PINS_MASK));		
	}

	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E);
	delay_us(delay_in_us);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);
	delay_us(5);

	if (0 == DISPLAY_DATA_PINS_INVERT )
	{
		/* No need to invert data in the port */
		// TODO: implement		
	} else
	{			
		buf = (command & 0x0F);
		DISPLAY_DATA_PINS_PORT = ((((buf & 0x1) << 3) | ((buf & 0x2) << 1) | ((buf & 0x4) >> 1) | ((buf & 0x8) >> 3)) << DISPLAY_DATA_PINS_SHIFT) | (DISPLAY_DATA_PINS_PORT & (~DISPLAY_DATA_PINS_MASK));		
	}

	/* E = A7 */
	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E);
	delay_us(delay_in_us);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);
	delay_us(5);
	
#else
	#error
#endif

}

void displaySendData(uint8_t data)
{
	uint8_t buf = 0;
	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_RS ); // 0 - command, 1 - data 
	
#ifdef DISPLAY_MODE_8BIT	

	DISPLAY_DATA_PINS_PORT = data;
			
	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E );
	delay_us(40);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);

#elif defined DISPLAY_MODE_4BIT

	if (0 == DISPLAY_DATA_PINS_INVERT )
	{
		/* No need to invert data in the port */
		// TODO: implement		
	} else
	{			
		buf = (data & 0xF0) >> 4;
		DISPLAY_DATA_PINS_PORT = ((((buf & 0x1) << 3) | ((buf & 0x2) << 1) | ((buf & 0x4) >> 1) | ((buf & 0x8) >> 3)) << DISPLAY_DATA_PINS_SHIFT) | (DISPLAY_DATA_PINS_PORT & (~DISPLAY_DATA_PINS_MASK));		
	}


	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E);
	delay_us(40);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);

	if (0 == DISPLAY_DATA_PINS_INVERT )
	{
		/* No need to invert data in the port */
		// TODO: implement		
	} else
	{				
		buf = (data & 0x0F);
		DISPLAY_DATA_PINS_PORT = ((((buf & 0x1) << 3) | ((buf & 0x2) << 1) | ((buf & 0x4) >> 1) | ((buf & 0x8) >> 3)) << DISPLAY_DATA_PINS_SHIFT) | (DISPLAY_DATA_PINS_PORT & (~DISPLAY_DATA_PINS_MASK));		
	}

	/* E = A7 */
	DISPLAY_CONFIG_PINS_PORT |= (1 << DISPLAY_CONFIG_PIN_E);
	delay_us(40);
	DISPLAY_CONFIG_PINS_PORT &= ~(1 << DISPLAY_CONFIG_PIN_E);

#else
	#error
#endif

}

char displayTranslateSymb(const char symb)
{
	switch (symb)
	{

		/* Special symbols */
		case '|':
				return 0xff;

		default:
			break;
	}
	return symb;
}

static inline void delay_ms(uint16_t count) { 
  while(count--) { 
    _delay_ms(1);

  } 
} 

static inline void delay_us(uint16_t count) { 
  while(count--) { 
    _delay_us(1); 

  } 
}

