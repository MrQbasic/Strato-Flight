#include "display.hpp"
#include "sensors.hpp"
#include "gps.hpp"
#include "storage.hpp"
#include "Wireless/Wireless.hpp"

#include <Wire.h>

//BUTTON FOR MENUS
#define PIN_USR 0

//SPI for SD card and LORA
#define PIN_SPI_MOSI   10
#define PIN_SPI_MISO   11
#define PIN_SPI_SCK    9

#define PIN_SPI2_MOSI  17
#define PIN_SPI2_MISO  7
#define PIN_SPI2_SCK   6

SPIClass spi(HSPI);
SPIClass spi2(FSPI);

//screen switching function
bool shouldSwitchPage = true;
void IRAM_ATTR switchWindows(){
    shouldSwitchPage = true;
}


void setup(){
    delay(1000);  //hotfix: enables programming without resetting the device manually thorugh the button
    
    Serial.begin(115200);

    Display::init();

    //init I2C 
    Wire.begin(45, 46);

    //enable interrupt for the user button
    pinMode(PIN_USR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_USR), switchWindows, FALLING);

    spi.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);
    spi2.begin(PIN_SPI2_SCK, PIN_SPI2_MISO, PIN_SPI2_MOSI);

    Sensors::init();

    //init GPS
    GPS::init();

    setupWireless(&spi);
    
    Storage::init_storage(&spi2);

}

int freq = 400;

void loop(){
    GPS::updateData();

    Sensors::update();

    Display::render(shouldSwitchPage);
    shouldSwitchPage = false;

    Storage::saveData();

    delay(400);

    sendLiveData();
}