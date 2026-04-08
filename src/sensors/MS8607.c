#include "MS8607.h"
#include "driver/i2c.h"

#include <stdarg.h>

//commands for humidity
uint8_t MS8607_I2C_HUMIDITY_CMD_RESET[]           = { 0xFE };
uint8_t MS8607_I2C_HUMIDITY_CMD_WRITE_USER[]      = { 0xE6, 0x00 };
uint8_t MS8607_I2C_HUMIDITY_CMD_READ_USER[]       = { 0xE7 };
uint8_t MS8607_I2C_HUMIDITY_CMD_MEASURE_HOLD[]    = { 0xE5 };
uint8_t MS8607_I2C_HUMIDITY_CMD_MEASURE_NO_HOLD[] = { 0xF5 };
uint8_t MS8607_I2C_HUMIDITY_CMD_PROM_READ[]       = { 0xA0 };

//commands for pressure and temp
uint8_t MS8607_I2C_P_T_CMD_RESET[]               = { 0x1E };
uint8_t MS8607_I2C_P_T_CMD_ADC_READ[]            = { 0x00 };
uint8_t MS8607_I2C_P_T_CMD_PROM_READ[]           = { 0xA0 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_256[]  = { 0x40 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_512[]  = { 0x42 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_1024[] = { 0x44 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_2048[] = { 0x46 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_4096[] = { 0x48 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_8192[] = { 0x4A };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_256[]  = { 0x50 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_512[]  = { 0x52 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_1024[] = { 0x54 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_2048[] = { 0x56 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_4096[] = { 0x58 };
uint8_t MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_8192[] = { 0x5A };

i2c_port_t MS8607_i2c_port = -1;

uint16_t MS8606_CALIBRATION_DATA[6];

bool MS8607_init(i2c_port_t i2c_port){
    vTaskDelay(pdMS_TO_TICKS(5000)); //wait for sensor to power up

    MS8607_i2c_port = i2c_port;

    //reset the sensor over both ports
    if( i2c_master_write_read_device(i2c_port, MS8607_I2C_ADDR_HUMIDITY, MS8607_I2C_HUMIDITY_CMD_RESET, sizeof(MS8607_I2C_HUMIDITY_CMD_RESET), NULL, 0, pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607 Reset 1\n");
        return true;
    }
    if( i2c_master_write_read_device(i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_RESET, sizeof(MS8607_I2C_P_T_CMD_RESET), NULL, 0, pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607 Reset 2\n");
        return true;
    }

    //read pressure and temp calibration data
    for(int i = 0; i <= 6; i++){
        uint8_t cmd = MS8607_I2C_P_T_CMD_PROM_READ[0] + (i << 1); 
        if( i2c_master_write_read_device(i2c_port, MS8607_I2C_ADDR_P_T, &cmd, sizeof(cmd), (uint8_t*)&MS8606_CALIBRATION_DATA[i], sizeof(uint16_t), pdMS_TO_TICKS(100)) != ESP_OK){
            printf("Failed to init MS8607 PROM READ\n");
            return true;
        }
        printf("PROM read %d: %04X\n", i, MS8606_CALIBRATION_DATA[i]);
    }

    return false;
}