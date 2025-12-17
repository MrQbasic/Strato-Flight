#include "gps.hpp"
#include "display.hpp"

enum Display::StatusBarLevel status_gps = Display::STATUS_BAD;

#define PIN_VGNSS_CTRL 3

namespace GPS {
    void init(){
        //enable the GPS module communication
        pinMode(PIN_VGNSS_CTRL, OUTPUT);
        digitalWrite(PIN_VGNSS_CTRL, HIGH);
        Serial1.begin(115200, SERIAL_8N1, 33, 34);

        Display::addStatusBar(&status_gps, "GPS");
    }

    TinyGPSPlus GPS;

    void updateData(){
        //caputure GPS data
        while (Serial1.available() > 0){
            GPS.encode(Serial1.read());
        }
        //evaluate Connection quality
        if(!GPS.hdop.isValid() || !GPS.location.isValid()){
            status_gps = Display::STATUS_BAD;
        } if(GPS.hdop.hdop() < 2.5) {
            status_gps = Display::STATUS_OK;
        } if(GPS.hdop.hdop() < 15.0){
            status_gps = Display::STATUS_MID;
        } else {
            status_gps = Display::STATUS_BAD;
        }
    }
}