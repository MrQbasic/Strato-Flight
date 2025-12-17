#include "display.hpp"
#include "HT_st7735.h"

#include "storage.hpp"

#include <vector>
#include <utility>

#include "windows/air-Data.hpp"
#include "windows/light-Data.hpp"
#include "windows/gps-Data.hpp"
#include "windows/storage-Info.hpp"

HT_st7735 st7735;

void Display::init(){
    st7735.st7735_init();
    st7735.st7735_fill_screen(ST7735_BLACK);
}


std::vector<std::pair<enum Display::StatusBarLevel*, String>> statusBars;

using windowRenderFunc = void(*)(HT_st7735);
windowRenderFunc renderFunction[] = {render_gpsData_1, render_gpsData_2, render_airData, render_LightData, render_storageInfo};

int pageCount = sizeof(renderFunction)/sizeof(renderFunction[0]);

int pageIndex = -1;

void Display::render(bool nextPage){
    //render Page
    if(nextPage){
        pageIndex ++;
        st7735.st7735_fill_screen(ST7735_BLACK);
        if(pageIndex >= pageCount){
            pageIndex = 0;
        }
        st7735.st7735_write_str(160-7*4, 0, (String)(pageIndex + 1 )+ "/" + (String)(pageCount), Font_7x10, ST7735_WHITE);
    }
    renderFunction[pageIndex](st7735);

    //render status bar
    int currentX = 0;
    for(int i=0; i<statusBars.size(); i++){
        auto &entry = statusBars[i];
        auto status = entry.first;
        auto text = entry.second;

        uint16_t color_bg, color_fg;
        switch(*status){
            case Display::STATUS_OK:
                color_bg = ST7735_GREEN;
                color_fg = ST7735_BLACK;
                break;
            case Display::STATUS_MID:
                color_bg = ST7735_YELLOW;
                color_fg = ST7735_BLACK;
                break;
            case Display::STATUS_BAD:
                color_bg = ST7735_RED;
                color_fg = ST7735_WHITE;
                break;
            case Display::STATUS_HIDE:
            default:
                Display::showMessage("ERROR: Unexpected Status", Display::MESSAGE_INFO);
                continue;
        }
        st7735.st7735_write_str(currentX, 79 - 10, text, Font_7x10, color_fg, color_bg);
        currentX += text.length() * 7 + 4;
    }
}


void Display::addStatusBar(enum Display::StatusBarLevel *status, String text){
    statusBars.push_back(std::make_pair(status, text));
}



//these are the background and text colors of the different message types (inverted is used for contrast)
int Message_Colors[3][2] = {{ST7735_WHITE, ST7735_BLACK},
                            {ST7735_YELLOW, ST7735_BLACK},
                            {ST7735_RED, ST7735_WHITE}};

String Message_Headline[3] = {"INFO:", "WARN:", "ERROR:"};

void Display::showMessage(String message, MessageLevel level, bool hang, bool saveLog){
    if(saveLog){
        String logLine = ((String) millis()) + " " + Message_Headline[level] + " " + message;
        Storage::saveLogLine(logLine);
    }
    st7735.st7735_fill_screen(Message_Colors[level][0]);
    st7735.st7735_write_str(0, 0, Message_Headline[level], Font_11x18, Message_Colors[level][1], Message_Colors[level][0]);
    st7735.st7735_write_str(0, 20, message, Font_11x18, Message_Colors[level][1], Message_Colors[level][0]);
    if(hang){
        while (1);
    }
    delay(2000);
    st7735.st7735_fill_screen(ST7735_BLACK);
}