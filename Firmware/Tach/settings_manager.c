#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "settings_manager.h"
#include "display.h"

/* EEPROM address list */
#define EEPROM_MAGIC_NUMBER_ADDR 2
#define EEPROM_SETTING_VOLTAGE_COMPENSATION_ADDR 4
#define EEPROM_SETTING_BACKLIGHT_INTENSITY_ADDR 6
#define EEPROM_SETTING_PULSES_PER_REVOLUTION_ADDR 7
#define EEPROM_SETTING_MAX_RPM_ADDR 8
#define EEPROM_SETTING_MIN_RPM_ADDR 10
#define EEPROM_SETTING_BACKLIGHT_TIMEOUT_ADDR 12
#define EEPROM_SETTING_MAX_VOLTAGE_ADDR 14
#define EEPROM_SETTING_MIN_VOLTAGE_ADDR 16
#define EEPROM_SETTING_BACKLIGHT_TACH_ON_ADDR 18
#define EEPROM_SETTING_BEEPER_ENABLED_ADDR 20
#define EEPROM_SETTING_MIN_VOLTAGE_ALARM_ON_ADDR 22
#define EEPROM_SETTING_MAX_VOLTAGE_ALARM_ON_ADDR 24
#define EEPROM_SETTING_MIN_RPM_ALARM_ON_ADDR 26
#define EEPROM_SETTING_MAX_RPM_ALARM_ON_ADDR 28

/* Other defines */
#define EEPROM_MAGIC_NUMBER 0xABCD

static uint8_t magic_written = 0;

static uint16_t voltage_compensation = 0;
static uint8_t backlight_intensity = DISPLAY_BACKLIGHT_TOP;
static uint8_t pulses_per_revolution = 1;
static uint16_t max_rpm = 7000;
static uint16_t min_rpm = 800;
static uint16_t backlight_timeout = DISPLAY_BACKLIGHT_TIMEOUT_ALWAYS_ON;
static uint16_t max_voltage = 990;
static uint16_t min_voltage = 695;
static uint8_t backlight_tach_on = SETTINGS_MANAGER_BACKLIGHT_ON_WHEN_RUNNING;
static uint8_t beeper_enabled = SETTINGS_MANAGER_BEEPER_ENABLED;
static uint8_t min_voltage_alarm_on = SETTINGS_MANAGER_ALARM_ENABLED;
static uint8_t max_voltage_alarm_on = SETTINGS_MANAGER_ALARM_ENABLED;
static uint8_t min_rpm_alarm_on = SETTINGS_MANAGER_ALARM_ENABLED;
static uint8_t max_rpm_alarm_on = SETTINGS_MANAGER_ALARM_ENABLED;

void settings_manager_init()
{
	// Read all settings from EEPROM here
	if (eeprom_read_word((void *)EEPROM_MAGIC_NUMBER_ADDR) == EEPROM_MAGIC_NUMBER){
		// ok magic number matches accept values
		magic_written = 1;
		
		voltage_compensation = eeprom_read_word((void *)EEPROM_SETTING_VOLTAGE_COMPENSATION_ADDR);
		backlight_intensity = eeprom_read_byte((uint8_t *)EEPROM_SETTING_BACKLIGHT_INTENSITY_ADDR);
		pulses_per_revolution = eeprom_read_byte((uint8_t *)EEPROM_SETTING_PULSES_PER_REVOLUTION_ADDR);
		max_rpm = eeprom_read_word((void *)EEPROM_SETTING_MAX_RPM_ADDR);
		min_rpm = eeprom_read_word((void *)EEPROM_SETTING_MIN_RPM_ADDR);
		backlight_timeout = eeprom_read_word((void *)EEPROM_SETTING_BACKLIGHT_TIMEOUT_ADDR);
		max_voltage = eeprom_read_word((void *)EEPROM_SETTING_MAX_VOLTAGE_ADDR);
		min_voltage = eeprom_read_word((void *)EEPROM_SETTING_MIN_VOLTAGE_ADDR);
		backlight_tach_on = eeprom_read_byte((uint8_t *)EEPROM_SETTING_BACKLIGHT_TACH_ON_ADDR);
		beeper_enabled = eeprom_read_byte((uint8_t *)EEPROM_SETTING_BEEPER_ENABLED_ADDR);
		min_voltage_alarm_on = eeprom_read_byte((uint8_t *)EEPROM_SETTING_MIN_VOLTAGE_ALARM_ON_ADDR);
		max_voltage_alarm_on = eeprom_read_byte((uint8_t *)EEPROM_SETTING_MAX_VOLTAGE_ALARM_ON_ADDR);
		min_rpm_alarm_on = eeprom_read_byte((uint8_t *)EEPROM_SETTING_MIN_RPM_ALARM_ON_ADDR);
		max_rpm_alarm_on = eeprom_read_byte((uint8_t *)EEPROM_SETTING_MAX_RPM_ALARM_ON_ADDR);
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
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_BACKLIGHT_INTENSITY_ADDR, backlight_intensity);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_PULSES_PER_REVOLUTION_ADDR, pulses_per_revolution);
	eeprom_write_word((void *) EEPROM_SETTING_MAX_RPM_ADDR, max_rpm);
	eeprom_write_word((void *) EEPROM_SETTING_MIN_RPM_ADDR, min_rpm);
	eeprom_write_word((void *) EEPROM_SETTING_BACKLIGHT_TIMEOUT_ADDR, backlight_timeout);
	eeprom_write_word((void *) EEPROM_SETTING_MAX_VOLTAGE_ADDR, max_voltage);
	eeprom_write_word((void *) EEPROM_SETTING_MIN_VOLTAGE_ADDR, min_voltage);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_BACKLIGHT_TACH_ON_ADDR, backlight_tach_on);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_BEEPER_ENABLED_ADDR, beeper_enabled);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_MIN_VOLTAGE_ALARM_ON_ADDR, min_voltage_alarm_on);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_MAX_VOLTAGE_ALARM_ON_ADDR, max_voltage_alarm_on);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_MIN_RPM_ALARM_ON_ADDR, min_rpm_alarm_on);
	eeprom_write_byte((uint8_t *) EEPROM_SETTING_MAX_RPM_ALARM_ON_ADDR, max_rpm_alarm_on);
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

uint8_t settings_manager_get_backlight_intensity()
{
	return backlight_intensity;
}

void settings_manager_set_backlight_intensity(uint8_t backlightIntensity)
{
	backlight_intensity = backlightIntensity;
	settings_manager_write_data();
}

uint8_t settings_manager_get_pulses_per_revolution()
{
	return pulses_per_revolution;	
}

void settings_manager_set_pulses_per_revolution(uint8_t pulsesPerRevolution)
{
	pulses_per_revolution = pulsesPerRevolution;
	settings_manager_write_data();
}

uint16_t settings_manager_get_max_rpm()
{
	return max_rpm;
}

void settings_manager_set_max_rpm(uint16_t maxRPM)
{
	max_rpm = maxRPM;
	settings_manager_write_data();
}

uint16_t settings_manager_get_min_rpm()
{
	return min_rpm;
}

void settings_manager_set_min_rpm(uint16_t minRPM)
{
	min_rpm = minRPM;
	settings_manager_write_data();
}

uint16_t settings_manager_get_backlight_timeout()
{
	return backlight_timeout;
}

void settings_manager_set_backlight_timeout(uint16_t backlightTimeout)
{
	backlight_timeout = backlightTimeout;
	settings_manager_write_data();
}

uint16_t settings_manager_get_max_voltage()
{
	return max_voltage;
}

void settings_manager_set_max_voltage(uint16_t maxVOLTAGE)
{
	max_voltage = maxVOLTAGE;
	settings_manager_write_data();
}

uint16_t settings_manager_get_min_voltage()
{
	return min_voltage;
}

void settings_manager_set_min_voltage(uint16_t minVOLTAGE)
{
	min_voltage = minVOLTAGE;
	settings_manager_write_data();
}

uint8_t settings_manager_get_backlight_tach_on()
{
	return backlight_tach_on;
}

void settings_manager_set_backlight_tach_on(uint8_t backlightTachOn)
{
	backlight_tach_on = backlightTachOn;
	settings_manager_write_data();
}

uint8_t settings_manager_get_beeper_enabled()
{
	return beeper_enabled;	
}

void settings_manager_set_beeper_enabled(uint8_t beeperEnabled)
{
	beeper_enabled = beeperEnabled;
	settings_manager_write_data();	
}

uint8_t settings_manager_get_min_voltage_alarm_on()
{
	return min_voltage_alarm_on;
}

void settings_manager_set_min_voltage_alarm_on(uint8_t minVoltageAlarmOn)
{
	min_voltage_alarm_on = minVoltageAlarmOn;
	settings_manager_write_data();	
}

uint8_t settings_manager_get_max_voltage_alarm_on()
{
	return max_voltage_alarm_on;	
}

void settings_manager_set_max_voltage_alarm_on(uint8_t maxVoltageAlarmOn)
{
	max_voltage_alarm_on = maxVoltageAlarmOn;
	settings_manager_write_data();
}

uint8_t settings_manager_get_min_rpm_alarm_on()
{
	return min_rpm_alarm_on;	
}

void settings_manager_set_min_rpm_alarm_on(uint8_t minRPMAlarmOn)
{
	min_rpm_alarm_on = minRPMAlarmOn;
	settings_manager_write_data();	
}

uint8_t settings_manager_get_max_rpm_alarm_on()
{
	return max_rpm_alarm_on;
}

void settings_manager_set_max_rpm_alarm_on(uint8_t maxRPMAlarmOn)
{
	max_rpm_alarm_on = maxRPMAlarmOn;
	settings_manager_write_data();
}
