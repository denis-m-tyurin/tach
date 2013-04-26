#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include "power_monitor.h"

static int16_t voltage_compensation = 0;
static uint16_t voltage_approx = 0;

void power_monitor_init()
{
	/* Enable ADC */
	ADCSRA |= (1 << ADEN);
	/* ADC-1 
	 * Compare ADC IN to internal 2.56V reference source
	 * ADC value: from 0 (0V) to 3FF (2.56V) */
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<MUX0);
}

uint16_t power_monitor_get_voltage()
{
	uint16_t voltage = 0;
	
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
	//voltage_approx &= 0xFFFC;
	
	/* Calculate voltage */
	if (0 == voltage_approx)
	{
		/* The first time when we read voltage. Return the RAW value */
		voltage_approx = voltage + voltage_compensation;		
	}
	else
	{
		/* Increase or decrease voltage slowly to smooth ADC peaks */
		if (voltage_approx > (voltage + voltage_compensation))
		{
			voltage_approx--;
		}			
		else
		{
			voltage_approx++;
		}
	}
	
	/* voltage_approx value varies from 0 (0V) to 3FF (2.56V). 1 = 0,0025 v
	 * Source voltage is divided by 10K / 2K resistors. U ADC REF = 12 * (2/12)
	 * Which means 12V would be 2V on ADC pin */

	return voltage_approx;
}

void power_monitor_set_voltage_compensation(int16_t compensation)
{
	voltage_compensation = compensation;
}