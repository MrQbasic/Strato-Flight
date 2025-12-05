#include "sensors.hpp"

#include "HT_st7735.h"

void render_temp(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "Temp-Ext:", Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 12, "TMP: " + (String) Sensors::data.temp_ext_C, Font_7x10, ST7735_WHITE );
}