#include "sensors.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sensors/MS8607.h"
#include "sensors/LM75.h"
#include "sensors/VEML6070.h"

#include <stdio.h>

/*
#include "display.hpp"
#include <Wire.h>

#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_VEML6070.h>

#define LM75_ADDR 0x48

Adafruit_MS8607 ms8607;

Adafruit_VEML6070 uv = Adafruit_VEML6070();
*/

Sensor_Data sensor_data;

bool handle_I2C_Error(esp_err_t err){
    if(err != ESP_OK){
        printf("I2C ERROR: %d\n", err);
        return true;
    }
    return false;
}

bool sensors_init(){
    //setup I2C port
    bool err = false;
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SENSORS_I2C_SDA,
        .scl_io_num = SENSORS_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    err |= handle_I2C_Error(i2c_param_config(SENSORS_I2C_PORT, &i2c_config));
    err |= handle_I2C_Error(i2c_driver_install(SENSORS_I2C_PORT, i2c_config.mode, 0, 0, 0));
    if(err){
        printf("Failed to initialize I2C\n");
        return false;
    }

    err |= MS8607_init(SENSORS_I2C_PORT);

    err |= VEML6070_init(SENSORS_I2C_PORT);

    return err;
}

void sensors_update(){
    while(1){
        //vTaskDelay(pdMS_TO_TICKS(SENSORS_UPDATE_INTERVAL_MS));

        MS8607_read(&(sensor_data.temp_ext_C), &(sensor_data.pressure_ext), &(sensor_data.humidity_ext));
        
        if(LM75_getTemp(SENSORS_I2C_PORT, &(sensor_data.temp_ext_C_2))){
            printf("ERROR: sensor: LM75\n");
            continue;
        }

        if(VEML6070_readUV(&(sensor_data.uv_index))){
            printf("ERROR: sensor: VEML6070\n");
            continue;
        }

        printf("%.2f %.2f %.2f %.2f %.2f\n", sensor_data.temp_ext_C, sensor_data.temp_ext_C_2, sensor_data.humidity_ext, sensor_data.pressure_ext, sensor_data.uv_index);
    }
}



void sensors_start(){
    xTaskCreate(sensors_update, "sensor_update", 2048, NULL, 3, NULL);
}