#include "./Wireless/Modules/Wifi.hpp"

#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char* html = 
"<!DOCTYPE html>" 
"<html>"
"   <head>"
"       <title>Strato-Flight</title>"
"       <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"   </head>"
"   <body>"
"       <h1>Strato-Flight</h1>"
"       <p>Welcome to the Strato-Flight telemetry server!</p>"  
"   </body>"
"</html>";

void handleRoot(){
    server.send(200, "text/html", html);
}

void handleNotFound(){
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
}

bool wireless_wifi_init(){
    bool ok = true;

    ok &= WiFi.mode(WIFI_AP);
    ok &= WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
    ok &= WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound); 
    server.begin();
    return ok;
}


void wireless_wifi_sendData(char* data, int length){
    server.handleClient();
}