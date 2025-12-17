#include "display.hpp"
#include "sensors.hpp"
#include "gps.hpp"
#include "storage.hpp"

#include <SPI.h>

#include <Wire.h>

//BUTTON FOR MENUS
#define PIN_USR 0

//screen switching function
bool shouldSwitchPage = true;
void IRAM_ATTR switchWindows(){
    shouldSwitchPage = true;
}


void setup(){
    delay(1000);  //hotfix: enables programming without resetting the device manually thorugh the button

    Serial.begin(115200);
    Display::init();

    Wire.begin(45, 46);

    //enable interrupt
    pinMode(PIN_USR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_USR), switchWindows, FALLING);

    Sensors::init();

    //init GPS
    GPS::init();

    Storage::init();

}


void loop(){
    GPS::updateData();

    Sensors::update();

    Display::render(shouldSwitchPage);
    shouldSwitchPage = false;

    Storage::saveData();
}