#include "driver/i2c.h"

bool VEML6070_init(i2c_port_t i2c_port);

bool VEML6070_readUV(float* UV);