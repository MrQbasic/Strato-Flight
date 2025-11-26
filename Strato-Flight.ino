#include "Arduino.h"
#include "HT_st7735.h"
#include "HT_TinyGPS++.h"

//defines for GPS
TinyGPSPlus GPS;
#define VGNSS_CTRL 3

//defines for Display
HT_st7735 st7735;


//this keep time in second
uint time_current = -1;
uint time_start = -1;        
uint time_flying  = -1;

//error log
bool error_GPS_drop = false;


//switching windows with interrupt on user button
#define MaxWindows 3
#define windowSwitchPin 0
#define WindowIndex_PosY 80-11
#define WindowIndex_PosX 160-(8*3)
int windowIndex = 0;
bool windowUpdated = false;
//int handler function
void IRAM_ATTR switchWindows(){
  windowUpdated = true;
  windowIndex = (windowIndex + 1) % (MaxWindows);
}



int GPS_status = -1;
bool GPS_inited = false;

#define GPS_statusText_posX 160-(8*3)
#define GPS_statusText_posY 0

void updateGPS(){
    while(Serial1.available() > 0){
        GPS.encode(Serial1.read());
    }
    //check if we have a meaninfull sat signal
    int satCount = GPS.satellites.value();
    if(satCount < 4) {
        //check if we just started with the connection or if we droppes signal
        if(GPS_status > 0){
        error_GPS_drop = true;
        }
        //contine as normal
        GPS_status = 0;
        st7735.st7735_write_str(GPS_statusText_posX, GPS_statusText_posY, "GPS", Font_7x10, ST7735_WHITE, ST7735_RED);
    }else if(satCount < 10){
        GPS_status = 1;
        st7735.st7735_write_str(GPS_statusText_posX, GPS_statusText_posY, "GPS", Font_7x10, ST7735_BLACK, ST7735_YELLOW);
    }else{
        GPS_status = 2;
        st7735.st7735_write_str(GPS_statusText_posX, GPS_statusText_posY, "GPS", Font_7x10, ST7735_BLACK, ST7735_GREEN);
    }

    //calculate the current time in seconds past 00:00      //-> this is ok cuz we have launch windows within one day
    time_current = GPS.time.hour() * 3600;
    time_current += GPS.time.minute() * 60;
    time_current += GPS.time.second();

    //check if we do this for the first time
    if(!GPS_inited && GPS.location.isValid() && GPS.location.isUpdated()){
        GPS_inited = true;
        time_start = time_current;
    }

    time_flying = time_current - time_start;

}

bool startupCheck(){


  return true;
}

void setup() {
  //setup Display
  st7735.st7735_init();
  st7735.st7735_fill_screen(ST7735_BLACK);

  //setup GPS
  pinMode(VGNSS_CTRL,OUTPUT);
	digitalWrite(VGNSS_CTRL,HIGH);
	Serial1.begin(115200,SERIAL_8N1,33,34);

  //attach windowSwitching button
  pinMode(windowSwitchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(windowSwitchPin), switchWindows, RISING);

  Serial.begin(115200);

  startupCheck();
}


void loop() {
    updateGPS();

    //we update the windows every second or if we force a window switch
    if(GPS.time.second() % 1 == 0 || windowUpdated){
        if(windowUpdated){
            st7735.st7735_fill_screen(ST7735_BLACK);
            windowUpdated = false;
        }
        switch(windowIndex){
            case 0:
            render_GPS_Data();
            break;
            case 1:
            render_FlightData();
            break;
            case 2:
            render_ErrorData();
            break;
            default:
            break;
        }
    }
        
	
  char buf[5];
  snprintf(buf, 5, "%d/%d",windowIndex+1, MaxWindows);
  st7735.st7735_write_str(WindowIndex_PosX, WindowIndex_PosY, buf, Font_7x10, ST7735_WHITE);
 }



