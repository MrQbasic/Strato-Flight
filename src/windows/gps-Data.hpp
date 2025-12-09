#include "gps.hpp"
#include "HT_st7735.h"

void render_gpsData_1(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "GPS-Data_1/2:", Font_7x10, ST7735_WHITE);  
    screen.st7735_write_str(0, 12, "LAT: " + (String) GPS::GPS.location.lat(), Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 24, "LON: " + (String) GPS::GPS.location.lng(), Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 36, "ALT: " + (String) GPS::GPS.altitude.meters(), Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 48, "SPD: " + (String) GPS::GPS.speed.kmph(), Font_7x10, ST7735_WHITE );
}

void render_gpsData_2(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "GPS-Data_2/2:", Font_7x10, ST7735_WHITE); 
    screen.st7735_write_str(0, 12, "TIME: " + (String) GPS::GPS.time.hour() + ":" + (String) GPS::GPS.time.minute() + ":" + (String) GPS::GPS.time.second(), Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 24, "SAT: " + (String) GPS::GPS.satellites.value(), Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 36, "ACC: " + (String) (GPS::GPS.hdop.hdop() * 1.5), Font_7x10, ST7735_WHITE );
}