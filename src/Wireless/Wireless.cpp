#include "Wireless.hpp"

#include "Modules/Lora.hpp"

#include "display.hpp"
#include "sensors.hpp"
#include "gps.hpp"

#include <stddef.h>

char* liveDataBuffer;
int liveDataBufferLength = sizeof(Sensors::Data) + sizeof(double) * 3 + sizeof(int) * 2; //add some extra space for debug string

int packetID = 0;
int lastPacketStatus = -1;

enum Display::StatusBarLevel status_lora = Display::STATUS_BAD;

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

        int* liveDataBuffer_PacketParam = (int*) (liveDataBuffer_gps + 3);
        liveDataBuffer_PacketParam[0] = packetID;
        liveDataBuffer_PacketParam[1] = lastPacketStatus;
    }
}


void setupWireless(SPIClass *loraSPI) {
    wireless_lora_init(loraSPI);

    status_lora = Display::STATUS_MID;
    Display::addStatusBar(&status_lora, "LoRa");

    liveDataBuffer = (char*) malloc(liveDataBufferLength);
    Serial.println("Wireless buffer size: " + String(liveDataBufferLength));
}

int lastSendTime = 0;

typedef struct LoraResponse {
    int packet_id;
    uint8_t command;
    uint8_t arg;
    //redundant for data transmission
    uint8_t command_repeat;
    uint8_t arg_repeat;
} __attribute__((packed)) LoraResponse;

LoraResponse response;

void sendLiveData() {
    if(millis() - lastSendTime > 3000 || millis() < lastSendTime){ //send data every second, also handle millis overflow
        lastSendTime = millis();
        getLiveData(false);
        packetID++;
        
        wireless_lora_txData(liveDataBuffer, liveDataBufferLength);
        vTaskDelay(pdMS_TO_TICKS(500)); // delay to let the module tx properly

        if(!wireless_lora_rxData((uint8_t*) &response, sizeof(response))){
            if(response.packet_id == (packetID-1)){
                lastPacketStatus = 0;

                status_lora = Display::STATUS_OK;

                //ToDo handle commands!
            }else{
                status_lora = Display::STATUS_MID;
                lastPacketStatus = -1;

                Serial.println("packetID error!");
                Serial.println(packetID);
                Serial.println(response.packet_id);
            }
        } else {
            status_lora = Display::STATUS_MID;
            lastPacketStatus = -1;
            //no packet has been recived
        }
    }
}