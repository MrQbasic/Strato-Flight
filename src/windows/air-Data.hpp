#include "sensors.hpp"

#include "HT_st7735.h"

void render_airData(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "Air-Data:", Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 12, "TMP: " + (String) Sensors::data.temp_ext_C, Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 24, "TMP2: " + (String) Sensors::data.temp_ext_C_2, Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 36, "PRES: " + (String) Sensors::data.pressure_ext, Font_7x10, ST7735_WHITE );
    screen.st7735_write_str(0, 48, "HUM: " + (String) Sensors::data.humidity_ext, Font_7x10, ST7735_WHITE );    
}