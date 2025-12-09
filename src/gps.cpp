#include "gps.hpp"

namespace GPS {
    TinyGPSPlus GPS;

    void updateData(){
        while (Serial1.available() > 0){
            GPS.encode(Serial1.read());
        }
    }
}