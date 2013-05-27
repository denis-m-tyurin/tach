#include "one_wire.h"

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

/* NEVER PULL-UP PD7 as it is GND'ed */
#define PD7 #error

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


one_wire_bus_data_t* one_wire_initialize_bus(one_wire_port_t port, int pin)
{
	one_wire_bus_data_t *pData = malloc(sizeof(one_wire_bus_data_t));
	if (NULL == pData)
		return NULL;
	pData->port = port;
	pData->pin 	= pin;

	/* Configure the pin as pulled-up output by default, to charge capacitors of devices */
	switch (pData->port)
	{
		case ONE_WIRE_PORT_A:
			//DDRA |= 1 << pData->pin;
			//PORTA |= (1 << pData->pin);
			break;
		case ONE_WIRE_PORT_B:
			DDRB |= 1 << pData->pin;
			PORTB |= (1 << pData->pin);
			break;
		case ONE_WIRE_PORT_C:
			DDRC |= 1 << pData->pin;
			PORTC |= (1 << pData->pin);
			break;
		case ONE_WIRE_PORT_D:
			DDRD |= 1 << pData->pin;
			PORTD |= (1 << pData->pin);
			break;

		default:
			/* Something went wrong */
			free(pData);
			return NULL;
	}

	return pData;
}

bool one_wire_check_presence(const one_wire_bus_data_t* pBus)
{
	bool res = false;

	switch (pBus->port)
	{
		case ONE_WIRE_PORT_A:
			/* Mux PIN as pulled down out */ 
			DDRA |= 1 << pBus->pin;
			PORTA &= ~(1 << pBus->pin);
			/* Pause 480 us */
			_delay_us(480);
			/* Mux PIN as input */
			DDRA &= ~(1 << pBus->pin);
			/* Pause 70 us */
			_delay_us(70);
			/* Normally PIN should be pulled-down by a remote device */
			if ((PINA & (1 << pBus->pin)) == 0x0)
				res = true;
			/* Pause 410 us */
			 _delay_us(410);		
			
			break;
		case ONE_WIRE_PORT_B:
			/* Mux PIN as pulled down out */ 			
			DDRB |= 1 << pBus->pin;
			PORTB &= ~(1 << pBus->pin);
			/* Pause 480 us */
			_delay_us(480);
			/* Mux PIN as input */
			DDRB &= ~(1 << pBus->pin);
			/* Pause 70 us */
			_delay_us(70);
			
			/* Normally PIN should be pulled-down by a remote device */
			if ((PINB & (1 << pBus->pin)) == 0x0)
				res = true;
			/* Pause 410 us */
			 _delay_us(410);		

			break;
		case ONE_WIRE_PORT_C:
			/* Mux PIN as pulled down out */ 			
			DDRC |= 1 << pBus->pin;
			PORTC &= ~(1 << pBus->pin);
			/* Pause 480 us */
			_delay_us(480);
			/* Mux PIN as input */
			DDRC &= ~(1 << pBus->pin);
			/* Pause 70 us */
			_delay_us(70);
			
			/* Normally PIN should be pulled-down by a remote device */
			if ((PINC & (1 << pBus->pin)) == 0x0)
				res = true;
			/* Pause 410 us */
			 _delay_us(410);	

			break;
		case ONE_WIRE_PORT_D:
			/* Mux PIN as pulled down out */ 			
			DDRD |= 1 << pBus->pin;
			PORTD &= ~(1 << pBus->pin);
			/* Pause 480 us */
			_delay_us(480);
			/* Mux PIN as input */
			DDRD &= ~(1 << pBus->pin);
			/* Pause 70 us */
			_delay_us(70);
			
			/* Normally PIN should be pulled-down by a remote device */
			if ((PIND & (1 << pBus->pin)) == 0x0)
				res = true;
			/* Pause 410 us */
			 _delay_us(410);	

			break;
	}

 return res;
}

void one_wire_send_command(const one_wire_bus_data_t* pBus, const unsigned char command)
{ 
	unsigned char i, data;
 
	data = command;
 
	for(i=0;i<8;i++)
	{

		if ((data&0x01)==0x01) 
		{
			/* Send '1' to the bus */
			switch (pBus->port)
			{
			case ONE_WIRE_PORT_A:
				/* Mux PIN as output */
				DDRA |= 1 << pBus->pin;
				PORTA &= ~(1 << pBus->pin);
				/* Pause 6 us */
				_delay_us(6);
				/* Mux PIN as input (release bus) */
				DDRA &= ~(1 << pBus->pin);
				/* Pause 64 us */
				_delay_us(64);			
				break;
			case ONE_WIRE_PORT_B:
				/* Mux PIN as output */
				DDRB |= 1 << pBus->pin;
				PORTB &= ~(1 << pBus->pin);
				/* Pause 6 us */
				_delay_us(6);
				/* Mux PIN as input (release bus) */
				DDRB &= ~(1 << pBus->pin);
				/* Pause 64 us */
				_delay_us(64);
				break;
			case ONE_WIRE_PORT_C:
				/* Mux PIN as output */
				DDRC |= 1 << pBus->pin;
				PORTC &= ~(1 << pBus->pin);
				/* Pause 6 us */
				_delay_us(6);
				/* Mux PIN as input (release bus) */
				DDRC &= ~(1 << pBus->pin);
				/* Pause 64 us */
				_delay_us(64);
				break;
			case ONE_WIRE_PORT_D:
				/* Mux PIN as output */
				DDRD |= 1 << pBus->pin;
				PORTD &= ~(1 << pBus->pin);
				/* Pause 6 us */
				_delay_us(6);
				/* Mux PIN as input (release bus) */
				DDRD &= ~(1 << pBus->pin);
				/* Pause 64 us */
				_delay_us(64);
				break;
			}
        } 
		else 
        {
			/* Send '0' to the bus */
			switch (pBus->port)
			{
			case ONE_WIRE_PORT_A:
				/* Mux PIN as pulled down output */
				DDRA |= 1 << pBus->pin;
				PORTA &= ~(1 << pBus->pin);
				/* Pause 60 us */
				_delay_us(60);
				/* Mux PIN as input (release bus) */
				DDRA &= ~(1 << pBus->pin);
				/* Pause 10 us */
				_delay_us(10);
				break;
			case ONE_WIRE_PORT_B:
				/* Mux PIN as pulled down output */
				DDRB |= 1 << pBus->pin;
				PORTB &= ~(1 << pBus->pin);
				/* Pause 60 us */
				_delay_us(60);
				/* Mux PIN as input (release bus) */
				DDRB &= ~(1 << pBus->pin);
				/* Pause 10 us */
				_delay_us(10);
				break;
			case ONE_WIRE_PORT_C:
				/* Mux PIN as pulled down output */
				DDRC |= 1 << pBus->pin;
				PORTC &= ~(1 << pBus->pin);
				/* Pause 60 us */
				_delay_us(60);
				/* Mux PIN as input (release bus) */
				DDRC &= ~(1 << pBus->pin);
				/* Pause 10 us */
				_delay_us(10);
				break;
			case ONE_WIRE_PORT_D:
				/* Mux PIN as pulled down output */
				DDRD |= 1 << pBus->pin;
				PORTD &= ~(1 << pBus->pin);
				/* Pause 60 us */
				_delay_us(60);
				/* Mux PIN as input (release bus) */
				DDRD &= ~(1 << pBus->pin);
				/* Pause 10 us */
				_delay_us(10);
				break;
			}
        }
  		data=data>>1;
	}
}

void one_wire_send_command_wait_done(const one_wire_bus_data_t* pBus, const unsigned char command, const uint16_t max_timeout_us)
{ 
	uint16_t wait_loops = 0;

	one_wire_send_command(pBus, command);

	/* As per the spec remote device will hold "0" on the bus until handling is in progress, so we will be waiting for "1" */
	switch (pBus->port)
	{
	case ONE_WIRE_PORT_A:
		while (one_wire_is_conversion_done(pBus) == false && wait_loops != max_timeout_us)
		{
			_delay_us(1);
			//wait_loops++;
		}
		break;
	case ONE_WIRE_PORT_B:
		while (one_wire_is_conversion_done(pBus) && wait_loops != max_timeout_us)
		{
			_delay_us(1);
			//wait_loops++;
		}
		break;
	case ONE_WIRE_PORT_C:
		while (one_wire_is_conversion_done(pBus) && wait_loops != max_timeout_us)
		{
			_delay_us(1);
			//wait_loops++;
		}
		break;
	case ONE_WIRE_PORT_D:
		while (one_wire_is_conversion_done(pBus) && wait_loops != max_timeout_us)
		{
			_delay_us(1);
			//wait_loops++;
		}
		break;
	}
}

bool one_wire_is_conversion_done(const one_wire_bus_data_t* pBus)
{
	/* As per the spec remote device will hold "0" on the bus until handling is in progress, and release the bus to "1" when done */
	switch (pBus->port)
	{
		case ONE_WIRE_PORT_A:
			return (((PINA & (1 << pBus->pin)) == 0x0) ? false : true);				
		case ONE_WIRE_PORT_B:
			return (((PINB & (1 << pBus->pin)) == 0x0) ? false : true);			
		case ONE_WIRE_PORT_C:
			return (((PINC & (1 << pBus->pin)) == 0x0) ? false : true);	
		case ONE_WIRE_PORT_D:
			return (((PIND & (1 << pBus->pin)) == 0x0) ? false : true);	
	}
	return false;
}


uint16_t one_wire_receive_data(const one_wire_bus_data_t* pBus)
{
	unsigned char bit_counter;
	uint16_t out_val = 0;

 	/* Receive 16 bits from the bus */
	for(bit_counter=0;bit_counter<16;bit_counter++)
	{
		switch (pBus->port)
		{
		case ONE_WIRE_PORT_A:
		 	/* Mux PIN as pulled down output */
			DDRA |= 1 << pBus->pin;
			PORTA &= ~(1 << pBus->pin);
			/* Pause 6 us */
	  		_delay_us(6);
			/* Mux PIN as input (release bus) */
			DDRA &= ~(1 << pBus->pin);
			/* Pause 9 us */
  			_delay_us(9);
 
  			if ((PINA & (1 << pBus->pin)) == 0x0)
				out_val &= ~(1<<bit_counter); // 0 on the bus
			else 
				out_val |= 1<<bit_counter;	// 1 on the bus
			/* Pause 55 us */
  			_delay_us(55);		
			break;
		case ONE_WIRE_PORT_B:
		 	/* Mux PIN as pulled down output */
			DDRB |= 1 << pBus->pin;
			PORTB &= ~(1 << pBus->pin);
			/* Pause 6 us */
	  		_delay_us(6);
			/* Mux PIN as input (release bus) */
			DDRB &= ~(1 << pBus->pin);
			/* Pause 9 us */
  			_delay_us(9);
 
  			if ((PINB & (1 << pBus->pin)) == 0x0)
				out_val &= ~(1<<bit_counter); // 0 on the bus
			else 
				out_val |= 1<<bit_counter;	// 1 on the bus
			/* Pause 55 us */
  			_delay_us(55);
			break;
		case ONE_WIRE_PORT_C:
			/* Mux PIN as pulled down output */
			DDRC |= 1 << pBus->pin;
			PORTC &= ~(1 << pBus->pin);
			/* Pause 6 us */
	  		_delay_us(6);
			/* Mux PIN as input (release bus) */
			DDRC &= ~(1 << pBus->pin);
			/* Pause 9 us */
  			_delay_us(9);
 
  			if ((PINC & (1 << pBus->pin)) == 0x0)
				out_val &= ~(1<<bit_counter); // 0 on the bus
			else 
				out_val |= 1<<bit_counter;	// 1 on the bus
			/* Pause 55 us */
  			_delay_us(55);
			break;
		case ONE_WIRE_PORT_D:
		 	/* Mux PIN as pulled down output */
			DDRD |= 1 << pBus->pin;
			PORTD &= ~(1 << pBus->pin);
			/* Pause 6 us */
	  		_delay_us(6);
			/* Mux PIN as input (release bus) */
			DDRD &= ~(1 << pBus->pin);
			/* Pause 9 us */
  			_delay_us(9);
 
  			if ((PIND & (1 << pBus->pin)) == 0x0)
				out_val &= ~(1<<bit_counter); // 0 on the bus
			else 
				out_val |= 1<<bit_counter;	// 1 on the bus
			/* Pause 55 us */
  			_delay_us(55);
		break;
		}
	}

	return out_val;
}

void one_wire_temperature_convert(one_wire_temperature_data_t* pTempData, uint16_t sourceData)
{
	if (NULL != pTempData)
	{
			/* Read sign bit */
			pTempData->is_positive = (((sourceData & (1 << 11)) == 1) ? ONE_WIRE_TEMPERATURE_NEGATIVE : ONE_WIRE_TEMPERATURE_POSITIVE);
			
			/* Calculate base */
			pTempData->degree_base = (sourceData & 0x7ff) >> 4;
			
			/* Calculate mantissa */
			pTempData->degree_mantissa = (sourceData & 0xf) * 625;
		
	}
}

