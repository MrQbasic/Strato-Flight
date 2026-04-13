#include "LM75.h"

bool LM75_getTemp(i2c_port_t i2c_port, float* temp_C){
    uint8_t cmd_Temp[] = {0x00};
    if(i2c_master_write_to_device(i2c_port, LM75_I2C_ADDR, cmd_Temp, 1, pdMS_TO_TICKS(100)) != ESP_OK) return true;
    
    uint8_t temp_buf[2];
    if(i2c_master_read_from_device(i2c_port, LM75_I2C_ADDR, temp_buf, 2, pdMS_TO_TICKS(100)) != ESP_OK) return true;
    int16_t temp = ((temp_buf[0] << 8) | temp_buf[1]) >> 7; 

    *temp_C = temp * 0.5f; 
    return false;
}