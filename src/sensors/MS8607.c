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
    printf("MS8706 Setup\n");
    MS8607_i2c_port = i2c_port;

    //reset the sensor over both ports
    if( i2c_master_write_to_device(i2c_port, MS8607_I2C_ADDR_HUMIDITY, MS8607_I2C_HUMIDITY_CMD_RESET, sizeof(MS8607_I2C_HUMIDITY_CMD_RESET), pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607 Reset 1\n");
        return true;
    }
    if( i2c_master_write_to_device(i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_RESET, sizeof(MS8607_I2C_P_T_CMD_RESET), pdMS_TO_TICKS(100)) != ESP_OK){
        printf("Failed to init MS8607 Reset 2\n");
        return true;
    }

    //read pressure and temp calibration data
    printf("MS8607 PROM Data:\n");
    for(int i = 1; i <= 6; i++){
        uint8_t cmd = MS8607_I2C_P_T_CMD_PROM_READ[0] + (i << 1); 
        uint8_t data[2];
        if( i2c_master_write_read_device(i2c_port, MS8607_I2C_ADDR_P_T, &cmd, sizeof(cmd), data, sizeof(data), pdMS_TO_TICKS(100)) != ESP_OK){
            printf("Failed to init MS8607 PROM READ\n");
            return true;
        }
        MS8606_CALIBRATION_DATA[i-1] = (data[0] << 8) | data[1];
        printf("0x%02x: %u\n", cmd, (unsigned int)MS8606_CALIBRATION_DATA[i-1]);
    }

    printf("MS8706 OK\n");
    return false;
}

bool MS8607_read(float* temp_C, float* pressure_hPa, float* humidity_percent){
    //get pressure data
    uint8_t pressure_data[3] = {0};
    i2c_master_write_to_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_CONVERT_D1_OSR_8192, 1, pdMS_TO_TICKS(100));
    vTaskDelay(pdMS_TO_TICKS(30));
    i2c_master_write_to_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_ADC_READ, 1, pdMS_TO_TICKS(100));
    i2c_master_read_from_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, pressure_data, 3, pdMS_TO_TICKS(100));
    uint32_t pressure_raw = (pressure_data[0] << 16) | (pressure_data[1] << 8) | pressure_data[2]; 

    //get temp data
    uint8_t temp_data[3] = {0};
    i2c_master_write_to_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_CONVERT_D2_OSR_8192, 1, pdMS_TO_TICKS(100));
    vTaskDelay(pdMS_TO_TICKS(30));
    i2c_master_write_to_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, MS8607_I2C_P_T_CMD_ADC_READ, 1, pdMS_TO_TICKS(100));
    i2c_master_read_from_device(MS8607_i2c_port, MS8607_I2C_ADDR_P_T, temp_data, 3, pdMS_TO_TICKS(100));
    uint32_t temp_raw = (temp_data[0] << 16) | (temp_data[1] << 8) | temp_data[2]; 

    //calc temp
    int32_t dT = temp_raw - ((uint32_t)MS8606_CALIBRATION_DATA[4] << 8);
    int32_t temp = 2000 + (( dT * (uint32_t)MS8606_CALIBRATION_DATA[5] ) >> 23 );

    //calc pressure
    int64_t off  = ((int64_t)MS8606_CALIBRATION_DATA[1] << 17) + (((int64_t) MS8606_CALIBRATION_DATA[3] * dT ) >> 6);  
    int64_t sens = ((int64_t)MS8606_CALIBRATION_DATA[0] << 16) + (((int64_t) MS8606_CALIBRATION_DATA[2] * dT ) >> 7);
    
    
    int64_t temp_2, off_2, sens_2;
    //do compensation
    if(temp < 2000){
        temp_2 = (4 * ((int64_t) dT * dT)) >> 33;
        off_2 = 61 * (int64_t)(temp-2000) * (temp-2000) >> 4;
        sens_2 = 29 * (int64_t)(temp-2000) * (temp-2000) >> 4;

        if(temp < -1500){
            off_2  += 17 * (int64_t)(temp + 1500) * (temp + 1500);
            off_2 += 9  * (int64_t)(temp + 1500) * (temp + 1500);
        }
    }else{
        temp_2 = (5 * ((int64_t) dT * dT)) >> 38; 
        off_2 = 0; 
        sens_2 = 0;
    }
    
    //apply compensation
    sens -= sens_2;
    off  -= off_2;
    int64_t pressure = ((( pressure_raw * sens) >> 21) - off) >> 15; 
    
    *temp_C       = (temp-temp_2) / 100.0f;
    *pressure_hPa = pressure / 100.0f;

    //read humidty
    uint8_t humidity_data[2];
    i2c_master_write_read_device(MS8607_i2c_port, MS8607_I2C_ADDR_HUMIDITY, MS8607_I2C_HUMIDITY_CMD_MEASURE_HOLD, 1, humidity_data, 2, pdMS_TO_TICKS(100));
    uint16_t humidity_raw = (humidity_data[0] << 8) | (humidity_data[1] & 0xFC);

    //apply compensation
    float RH = -6.0f + 125.0f * ((float)humidity_raw / (float)(1<<16));
    *humidity_percent = RH + (20.0f - *temp_C) * (-0.18f);

    return true;
}
