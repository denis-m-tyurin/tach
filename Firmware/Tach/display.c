
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include "display.h"
#include <avr/io.h>
#include <util/delay.h>
#include "settings_manager.h"

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

static uint8_t backlight_pwm_enabled = 0;
static uint16_t backlight_timeout = DISPLAY_BACKLIGHT_TIMEOUT_ALWAYS_ON;
static uint16_t backlight_remaining_timeot = 0;

/********************************************
 *      PUBLIC FUNCTIONS                    *
 *******************************************/

void display_set_timeout(uint16_t display_timeout)
{
	backlight_timeout = display_timeout;
	backlight_remaining_timeot = backlight_timeout;
}

void display_timeout_user_active()
{
	if (0 == backlight_remaining_timeot)
	{
		/* Enable backlight again */
		display_set_backlight(settings_manager_get_backlight_intensity());
	}
	backlight_remaining_timeot = backlight_timeout;
}

void display_timeout_1sec_tick()
{
	if (0 != backlight_remaining_timeot)
	{
		if (DISPLAY_BACKLIGHT_TIMEOUT_ALWAYS_ON != backlight_remaining_timeot)
		{
			backlight_remaining_timeot--;
		}

		if (0 == backlight_remaining_timeot)
		{
			display_set_backlight(DISPLAY_BACKLIGHT_OFF);
		}
	}	
}

void display_set_backlight( uint8_t backlight_level )
{
	/* Configure pin just in case */
	DISPLAY_BACKLIGHT_DDR |= (1 << DISPLAY_BACKLIGHT_PIN);
	
	if (0 == backlight_level)
	{
		/* Turn PWM off and set pin to 0 */
		if (1 == backlight_pwm_enabled)
		{
			TCCR1A &= ~(1 << COM1A1);
			backlight_pwm_enabled = 0;
		}
		DISPLAY_BACKLIGHT_PORT &= ~(1 << DISPLAY_BACKLIGHT_PIN);		
		
	} else if (DISPLAY_BACKLIGHT_TOP == backlight_level)
	{
		/* Turn PWM off and set pin to 1 */
		if (1 == backlight_pwm_enabled)
		{
			TCCR1A &= ~(1 << COM1A1);
			backlight_pwm_enabled = 0;
		}		

		DISPLAY_BACKLIGHT_PORT |= (1 << DISPLAY_BACKLIGHT_PIN);		
	} else
	{
		/* Enable PWM and set necessary pulse width
		 * Backlight pin is PD5 which is OC1A (Timer 1)
		 * Use Fast-PWM mode, counting up to MAX in ICR1, pin reset at OCR
		 * The timer itself is already set-up by beeper. ICR1 is set to 70
		 */
		if (0 == backlight_pwm_enabled)
		{
			TCCR1A |= (1 << COM1A1);			
			backlight_pwm_enabled = 1;
		}
		OCR1A = backlight_level;
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
	
	/* If string is less than line size, print spaces to overwrite old data */
	if (counter < DISPLAY_LINE_SIZE)
	{
		for (;counter<DISPLAY_LINE_SIZE; counter++)
		{
			displaySendData(' ');
		}
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
				
		/* Cyrillic */
		case 'À':
				return 0x41;		
		case 'Á':
				return 0xA0;	
		case 'Â':
				return 0x42;		
		case 'Ã':
				return 0xA1;
		case 'Ä':
				return 0xE0;
		case 'Å':
				return 0x45;
		case 'Æ':
				return 0xA3;
		case 'Ç':
				return 0xA4;
		case 'È':
				return 0xA5;
		case 'É':
				return 0xA6;
		case 'Ê':
				return 0x4B;
		case 'Ë':
				return 0xA7;
		case 'Ì':
				return 0x4D;
		case 'Í':
				return 0x48;
		case 'Î':
				return 0x4F;
		case 'Ï':
				return 0xA8;
		case 'Ð':
				return 0x50;
		case 'Ñ':
				return 0x43;
		case 'Ò':
				return 0x54;
		case 'Ó':
				return 0xA9;
		case 'Þ':
				return 0xB0;
		case 'X':
				return 0x58;
		case '×':
				return 0xAB;
		case 'Û':
				return 0xAE;
		case 'Ü':
				return 0x62;
		case 'ß':
				return 0xB1;
		case 'à':
				return 0x61;
		case 'á':
				return 0xb2;
		case 'â':
				return 0xb3;
		case 'ã':
				return 0xb4;
		case 'ä':
				return 0xe3;
		case 'å':
				return 0x65;		
		case 'è':
				return 0xb8;
		case 'ê':
				return 0xba;
		case 'ë':
				return 0xbb;
		case 'ì':
				return 0xbc;
		case 'í':
				return 0xbd;
		case 'î':
				return 0x6f;
		case 'ñ':
				return 0x63;
		case 'ó':
				return 0x79;
		case '÷':
				return 0xc0;
		case 'þ':
				return 0xc6;
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

