#include <stdbool.h>
#include "driver/i2c.h"

#define LM75_I2C_ADDR 0x48

bool LM75_getTemp(i2c_port_t i2c_port, float* temp_C);