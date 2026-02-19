#define WIFI_SSID       "STRATO-FLIGHT"
#define WIFI_PASSWORD   "stratoflight"

bool wireless_wifi_init();

void wireless_wifi_sendData(char* data, int length);