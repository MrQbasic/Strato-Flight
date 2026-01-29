#include <SPI.h>

#define WIFI_SSID       "STRATO-FLIGHT"
#define WIFI_PASSWORD   "stratoflight"

void setupWireless(SPIClass *loraSPI);

void sendLiveData();