#include "display.hpp"
#include "HT_st7735.h"


#include "windows/tempSensor.hpp"


HT_st7735 st7735;

void Display::init(){
    st7735.st7735_init();
    st7735.st7735_fill_screen(ST7735_BLACK);
}


using windowRenderFunc = void(*)(HT_st7735);
windowRenderFunc renderFunction[] = {render_temp};


int pageIndex = 0;

void Display::render(bool nextPage){
    renderFunction[pageIndex](st7735);
}


void Display::addStatusBar(int* status, String text){
    
}



//these are the background and text colors of the different message types (inverted is used for contrast)
int Message_Colors[3][2] = {{ST7735_WHITE, ST7735_BLACK},
                            {ST7735_YELLOW, ST7735_BLACK},
                            {ST7735_RED, ST7735_WHITE}};

String Message_Headline[3] = {"INFO:", "WARN:", "ERROR:"};

void Display::showMessage(String message, MessageLevel level){
    st7735.st7735_fill_screen(Message_Colors[level][0]);
    st7735.st7735_write_str(0, 0, Message_Headline[level], Font_11x18, Message_Colors[level][1], Message_Colors[level][0]);
    st7735.st7735_write_str(0, 20, message, Font_11x18, Message_Colors[level][1], Message_Colors[level][0]);
}