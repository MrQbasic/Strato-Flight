#include "sensors.hpp"
#include "display.hpp"
#include <Wire.h>

#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>

#define LM75_ADDR 0x48

Adafruit_MS8607 ms8607;


namespace Sensors {

    Data data;


    bool init(){
        if(!ms8607.begin()){
            Display::showMessage("IO ERROR: PRESSURE EXT", Display::MESSAGE_ERROR);
            while(1);
        }


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
    }

}