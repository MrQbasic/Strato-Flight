#include "Wireless/Modules/Lora.hpp"

#include "HT_st7735.h"

void render_wireless_lora(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "LoRa:", Font_7x10, ST7735_WHITE);

    screen.st7735_write_str(0, 12, "FREQ: " + String(LORA_RF_FREQUENCY/1000000.0, 3) +"MHz", Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 24, "BW:   " +(String)(LORA_BANDWIDTH), Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 36, "SF:   " +(String)(LORA_SPREADING_FACTOR), Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 48, "CR:   " +(String)(LORA_CODINGRATE), Font_7x10, ST7735_WHITE);
}