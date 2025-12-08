#include "sensors.hpp"

#include "HT_st7735.h"

void render_LightData(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "Light-Data:", Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 12, "UV Index: " + (String) Sensors::data.uv_index , Font_7x10, ST7735_WHITE);
}