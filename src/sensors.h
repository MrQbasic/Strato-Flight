#pragma once
#include <stdbool.h>
#include "driver/i2c.h"

#define SENSORS_UPDATE_INTERVAL_MS 1000

//I2C pins
#define SENSORS_I2C_SDA 45
#define SENSORS_I2C_SCL 4

#define SENSORS_I2C_PORT I2C_NUM_0

typedef struct Sensor_Data{
    float temp_ext_C;
    
    float temp_ext_C_2;
    float pressure_ext;
    float humidity_ext;
    float uv_index;
} Sensor_Data;

extern Sensor_Data sensor_data;

//returns false on error which gets shown on display
bool sensors_init();

//This starts the sensor loop which updates the values
void sensors_start();