#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum 
{
	ONE_WIRE_PORT_A = 0,
	ONE_WIRE_PORT_B,
	ONE_WIRE_PORT_C,
	ONE_WIRE_PORT_D,	
} one_wire_port_t;

typedef struct 
{
	one_wire_port_t port;
	int pin;

} one_wire_bus_data_t;

#define ONE_WIRE_TEMPERATURE_POSITIVE 1
#define ONE_WIRE_TEMPERATURE_NEGATIVE 0

typedef struct  
{
	uint8_t is_positive;
	uint8_t degree_base;
	uint16_t degree_mantissa;
} one_wire_temperature_data_t;

one_wire_bus_data_t* one_wire_initialize_bus(one_wire_port_t port, int pin);
bool one_wire_check_presence(const one_wire_bus_data_t* pBus);
void one_wire_send_command(const one_wire_bus_data_t* pBus, const unsigned char command);
void one_wire_send_command_wait_done(const one_wire_bus_data_t* pBus, const unsigned char command, const uint16_t max_timeout_us);
uint16_t one_wire_receive_data(const one_wire_bus_data_t* pBus);
void one_wire_temperature_convert(one_wire_temperature_data_t* pTempData, uint16_t sourceData);
bool one_wire_is_conversion_done(const one_wire_bus_data_t* pBus);

#endif /* ONE_WIRE_H */
