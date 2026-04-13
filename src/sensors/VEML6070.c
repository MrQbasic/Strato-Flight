#include "sensors/VEML6070.h"

i2c_port_t veml6070_i2c_port;

#define VEML6070_ADDR_WRITE  0x38
#define VEML6070_ADDR_LSB    0x38
#define VEML6070_ADDR_MSB    0x39

#define VEML6070_CMD_INIT    0x06

bool VEML6070_init(i2c_port_t i2c_port){
    veml6070_i2c_port = i2c_port;

    uint8_t cmd[] = {0x06}; //init command
    if(i2c_master_write_to_device(veml6070_i2c_port, VEML6070_ADDR_WRITE, cmd, 1, pdMS_TO_TICKS(100)) != ESP_OK) return true;
    
    return false;
}

bool VEML6070_readUV(float* UV){
    uint8_t buf1, buf2;
    if(i2c_master_read_from_device(veml6070_i2c_port, VEML6070_ADDR_MSB, &buf1, 1, pdMS_TO_TICKS(100)) != ESP_OK) return true;
    if(i2c_master_read_from_device(veml6070_i2c_port, VEML6070_ADDR_LSB, &buf2, 1, pdMS_TO_TICKS(100)) != ESP_OK) return true;
    
    *UV = (float) (((uint16_t)buf1 << 8) | buf2);

    return false;
}