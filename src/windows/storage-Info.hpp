#include "storage.hpp"

#include "HT_st7735.h"

void render_storageInfo(HT_st7735 screen){
    screen.st7735_write_str(0,  0, "Storage:", Font_7x10, ST7735_WHITE);


    int totalMB, usedMB;
    Storage::getSpaceInfo(&totalMB, &usedMB);
    screen.st7735_write_str(0, 12, "FREE: " +(String)(totalMB-usedMB) +"MB", Font_7x10, ST7735_WHITE);
    
    extern String path_dataFile;
    extern String path_logFile;
    screen.st7735_write_str(0, 30, path_dataFile, Font_7x10, ST7735_WHITE);
    screen.st7735_write_str(0, 42, path_logFile, Font_7x10, ST7735_WHITE);
    
}