#include "Wireless.hpp"

#include "Modules/Lora.hpp"

#include "display.hpp"
#include "sensors.hpp"
#include "gps.hpp"

#include <stddef.h>

char* liveDataBuffer;
int liveDataBufferLength = sizeof(Sensors::Data) + sizeof(double) * 3; //add some extra space for debug string

void getLiveData(bool debug=false){
    if(debug){
        String msg = "{";
        msg += "\"temp_ext_C\": " + String(Sensors::data.temp_ext_C) + ",";
        msg += "\"temp_ext_C_2\": " + String(Sensors::data.temp_ext_C_2) + ",";
        msg += "\"pressure_ext\": " +  String(Sensors::data.pressure_ext) + ",";
        msg += "\"humidity_ext\": " +  String(Sensors::data.humidity_ext) + ",";
        msg += "\"uv_index\": " +  String(Sensors::data.uv_index);
        msg += "}";
        size_t len = msg.length();
        if(len > liveDataBufferLength){
            free(liveDataBuffer);
            liveDataBufferLength = len + 1;
            liveDataBuffer = (char*) malloc(liveDataBufferLength);
        }
        msg.toCharArray(liveDataBuffer, len + 1);
    }else{
        //copy sensor data
        memcpy(liveDataBuffer, &Sensors::data, sizeof(Sensors::Data));
        
        //copy gps data
        double* liveDataBuffer_gps = (double*) (liveDataBuffer + sizeof(Sensors::Data));
        liveDataBuffer_gps[0] = GPS::GPS.location.lat();
        liveDataBuffer_gps[1] = GPS::GPS.location.lng();
        liveDataBuffer_gps[2] = GPS::GPS.altitude.meters();
    }
}


void setupWireless(SPIClass *loraSPI) {
    wireless_lora_init(loraSPI);

    liveDataBuffer = (char*) malloc(liveDataBufferLength);
    Serial.println("Wireless buffer size: " + String(liveDataBufferLength));
}

int lastSendTime = 0;

void sendLiveData() {
    if(millis() - lastSendTime > 3000 || millis() < lastSendTime){ //send data every second, also handle millis overflow
        lastSendTime = millis();
        getLiveData(false);
        wireless_lora_sendData(liveDataBuffer, liveDataBufferLength);
    }
}