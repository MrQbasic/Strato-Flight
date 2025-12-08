#include "sensors.hpp"
#include "display.hpp"
#include <Wire.h>

#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_VEML6070.h>

#define LM75_ADDR 0x48

Adafruit_MS8607 ms8607;

Adafruit_VEML6070 uv = Adafruit_VEML6070();

namespace Sensors {

    Data data;

    bool init(){
        if(!ms8607.begin()){
            Display::showMessage("IO ERROR: PRESSURE EXT", Display::MESSAGE_ERROR);
            while(1);
        }
        uv.begin(VEML6070_1_T);

        return true;
    }

    void update(){
        //read Temp data from module
        Wire.beginTransmission(LM75_ADDR);
        Wire.write(0x00);   // temperature register
        Wire.endTransmission(false);
        Wire.requestFrom(LM75_ADDR, 2);
        if (Wire.available() < 2){
            Display::showMessage("IO ERROR: TEMP EXT", Display::MESSAGE_ERROR);
            while(1);
        }
        uint8_t msb = Wire.read();
        uint8_t lsb = Wire.read();
        int16_t raw = ((msb << 8) | lsb) >> 7;  
        data.temp_ext_C = raw * 0.5f;
        
        //read Pressure module
        sensors_event_t temp, pressure, humidity;
        if(!ms8607.getEvent(&pressure, &temp, &humidity)){
            Display::showMessage("IO ERROR: PRESSURE EXT", Display::MESSAGE_ERROR);
            while(1);
        }    
        data.temp_ext_C_2 = temp.temperature;
        data.pressure_ext  = pressure.pressure;
        data.humidity_ext  = humidity.relative_humidity;
    
        //read UV data
        uint16_t uv_index = uv.readUV();
        if(uv_index == 0xFFFF){
            Display::showMessage("IO ERROR: UV", Display::MESSAGE_ERROR);
            while(1);
        }
        data.uv_index = uv_index / 15.0f;



    }

}