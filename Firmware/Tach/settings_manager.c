#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "settings_manager.h"

/* EEPROM address list */
#define EEPROM_MAGIC_NUMBER_ADDR 2
#define EEPROM_SETTING_VOLTAGE_COMPENSATION_ADDR 4

/* Other defines */
#define EEPROM_MAGIC_NUMBER 0xABCD

static uint8_t magic_written = 0;

static uint16_t voltage_compensation = 0;

void settings_manager_init()
{
	// Read all settings from EEPROM here
	if (eeprom_read_word((void *)EEPROM_MAGIC_NUMBER_ADDR) == EEPROM_MAGIC_NUMBER){
		// ok magic number matches accept values
		magic_written = 1;
		
		voltage_compensation = eeprom_read_word((void *)EEPROM_SETTING_VOLTAGE_COMPENSATION_ADDR);
				
		//eeprom_read_block((uint8_t *)gwip,(void *)41,sizeof(gwip));
		//eeprom_read_block((uint8_t *)udpsrvip,(void *)45,sizeof(udpsrvip));
		//udpsrvport=eeprom_read_word((void *)49);
		//alarmOn=eeprom_read_byte((uint8_t *)51);
		//heartbeat_timeout_sec=eeprom_read_word((void *)52);
		//dhcpOn=eeprom_read_byte((uint8_t *)54);
		//eeprom_read_block((uint8_t *)myip,(void *)55,sizeof(myip));
		//eeprom_read_block((char *)myname,(void *)59,sizeof(myname));
	}
	else
	{
		magic_written = 0;
	}
}

void settings_manager_write_data()
{
	if (0 == magic_written)
	{
		magic_written=1;
		eeprom_write_word((void *)EEPROM_MAGIC_NUMBER_ADDR, EEPROM_MAGIC_NUMBER); 
	}
	
	eeprom_write_word((void *) EEPROM_SETTING_VOLTAGE_COMPENSATION_ADDR, voltage_compensation);
}

uint16_t settings_manager_get_voltage_compensation()
{
	return voltage_compensation;
}

void settings_manager_set_voltage_compensation(uint16_t voltComp)
{
	voltage_compensation = voltComp;
	settings_manager_write_data();
}