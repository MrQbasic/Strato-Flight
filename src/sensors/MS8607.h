#include "driver/i2c.h"

#define MS8607_I2C_ADDR_P_T        0x76
#define MS8607_I2C_ADDR_HUMIDITY   0x40

//return true on error
bool MS8607_init(i2c_port_t i2c_port);

//returns true on error
bool MS8607_read(float* temp_C, float* pressure_hPa, float* humidity_percent);