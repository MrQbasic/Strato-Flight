#include "sensors.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#include "sensors/MS8607.h"

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
        return false;
    }
    return true;
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
    err |= handle_I2C_Error(i2c_param_config(I2C_NUM_0, &i2c_config));
    err |= i2c_driver_install(I2C_NUM_0, i2c_config.mode, 0, 0, 0);

    MS8607_init(I2C_NUM_0);

    /*if(!ms8607.begin()){
        Display::showMessage("I2C INIT ERROR: PRESSURE EXT", Display::MESSAGE_ERROR, true);
    }
    uv.begin(VEML6070_1_T);
    */
    return err;
}

void sensors_update(){
    /*
    //read Temp data from module
    Wire.beginTransmission(LM75_ADDR);
    Wire.write(0x00);   // temperature register
    Wire.endTransmission(false);
    Wire.requestFrom(LM75_ADDR, 2);
    if (Wire.available() < 2){
        Display::showMessage("I2C ERROR: TEMP EXT", Display::MESSAGE_ERROR);
    }
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t raw = ((msb << 8) | lsb) >> 7;  
    data.temp_ext_C = raw * 0.5f;
    
    //read Pressure module
    sensors_event_t temp, pressure, humidity;
    if(!ms8607.getEvent(&pressure, &temp, &humidity)){
        Display::showMessage("I2C ERROR: PRESSURE EXT", Display::MESSAGE_ERROR);
    }    
    data.temp_ext_C_2 = temp.temperature;
    data.pressure_ext  = pressure.pressure;
    data.humidity_ext  = humidity.relative_humidity;

    //read UV data
    uint16_t uv_index = uv.readUV();
    if(uv_index == 0xFFFF){
        Display::showMessage("I2C ERROR: UV", Display::MESSAGE_ERROR);
    }
    data.uv_index = uv_index / 15.0f;
    */
}
