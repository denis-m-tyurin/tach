#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "power_monitor.h"

uint8_t voltage_compensation = 0;

void power_monitor_init()
{
	/* Enable ADC */
	ADCSRA |= (1 << ADEN);
	/* ADC-1 */
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<MUX0);
}

uint16_t power_monitor_get_voltage()
{
	uint16_t voltage = 123;
	
	/* To start conversion write 1 to ADSC. It will be cleared by hardware when the conversion is completed. */
	/* Read ADCL at first. Once it is read ADC is blocked until ADCH is read. This is to ensure that a result of the same conversion is read */

	/* Start conversation */
	ADCSRA |=(1<<ADSC);
 
	/* Wait for some time (it takes about 18 ADC cycles */
	while ((ADCSRA &_BV(ADIF))==0x00) ;    
 
	/* Read result */
	voltage = ADCL;
	voltage |= ADCH<<8;
	
	// Skip the last two bits to digitally cancel noise
	//result &= 0xFFFC;
	
	/* Calculate voltage */
	return voltage + voltage_compensation;
}

void power_monitor_set_voltage_compensation(uint8_t compensation)
{
	voltage_compensation = compensation;
}