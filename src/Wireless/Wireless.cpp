/*

#include "Wireless.hpp"
#include <LoRa.h>

// EU LoRa frequency (868.1 MHz)
#define EU_FREQUENCY 868100000

void setupWireless() {
    // Initialize LoRa with EU frequency and PA_BOOST
    if (!LoRa.begin(EU_FREQUENCY, true)) {
        // Handle initialization failure
        while (1);
    }

    // Set spreading factor (default is 7, but for range, higher is better)
    LoRa.setSpreadingFactor(10);

    // Set signal bandwidth (125 kHz is common)
    LoRa.setSignalBandwidth(125E3);

    // Enable CRC
    LoRa.enableCrc();

    // Set sync word
    LoRa.setSyncWord(0x34);

    // Set TX power (14 dBm)
    LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
}

void sendLiveData() {
    // Example: Send a simple message
    LoRa.beginPacket();
    LoRa.print("Hello from Strato-Flight!");
    LoRa.endPacket();

    // Wait a bit before next send
    delay(1000);
}

*/