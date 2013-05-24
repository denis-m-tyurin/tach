#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

void settings_manager_init();
uint16_t settings_manager_get_voltage_compensation();
void settings_manager_set_voltage_compensation(uint16_t voltComp);
uint8_t settings_manager_get_backlight_intensity();
void settings_manager_set_backlight_intensity(uint8_t backlightIntensity);
uint8_t settings_manager_get_pulses_per_revolution();
void settings_manager_set_pulses_per_revolution(uint8_t pulsesPerRevolution);
uint16_t settings_manager_get_max_rpm();
void settings_manager_set_max_rpm(uint16_t maxRPM);
uint16_t settings_manager_get_min_rpm();
void settings_manager_set_min_rpm(uint16_t minRPM);
uint16_t settings_manager_get_backlight_timeout();
void settings_manager_set_backlight_timeout(uint16_t backlightTimeout);

#endif