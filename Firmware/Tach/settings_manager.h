#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

void settings_manager_init();
uint16_t settings_manager_get_voltage_compensation();
void settings_manager_set_voltage_compensation(uint16_t voltComp);

#endif