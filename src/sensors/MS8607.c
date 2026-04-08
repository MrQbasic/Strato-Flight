#include "MS8607.h"
#include "driver/i2c.h"

#include <stdarg.h>

uint8_t MS8607_I2C_HUMIDITY_CMD_RESET[]           = { 0xFE };
uint8_t MS8607_I2C_HUMIDITY_CMD_WRITE_USER[]      = { 0xE6, 0x00 };
uint8_t MS8607_I2C_HUMIDITY_CMD_READ_USER[]       = { 0xE7 };
uint8_t MS8607_I2C_HUMIDITY_CMD_MEASURE_HOLD[]    = { 0xE5 };
uint8_t MS8607_I2C_HUMIDITY_CMD_MEASURE_NO_HOLD[] = { 0xF5 };
uint8_t MS8607_I2C_HUMIDITY_CMD_PROM_READ[]       = { 0xA0 };

uint8_t MS8607_I2C_P_T_CMD_RESET[]                = { 0x1E };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_256[]           = { 0x1E };

i2c_port_t MS8607_i2c_port = -1;

bool MS8607_init(i2c_port_t i2c_port){
    MS8607_i2c_port = i2c_port;

    uint16_t read_buffer;

    //reset the sensor over both ports
    if( i2c_master_write_read_device(MS8607_i2c_port, MS8607_I2C_ADDR_HUMIDITY, MS8607_I2C_HUMIDITY_CMD_RESET, sizeof(MS8607_I2C_HUMIDITY_CMD_RESET), NULL, 0, pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607\n");
        return true;
    }
    if( i2c_master_write_read_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_RESET, sizeof(MS8607_I2C_P_T_CMD_RESET), NULL, 0, pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607\n");
        return true;
    }



    return false;
}