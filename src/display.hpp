#pragma once
#include "Arduino.h"

namespace Display{
    void init();


    //if the bool is set it automatically renders the next page
    void render(bool nextPage);


    enum StatusBarLevel{
        STATUS_HIDE,   //NOTHING AT ALL
        STATUS_BAD,    //RED
        STATUS_MID,    //YELLOW
        STATUS_OK      //GREEN
    };

    //add a status bar which needs to be rendered
    //returns true if added successfully
    void addStatusBar(enum StatusBarLevel *status, String text);

    //this shows the level of any direct screen message
    enum MessageLevel{
        MESSAGE_INFO,
        MESSAGE_WARN,
        MESSAGE_ERROR
    };

    //shows an error message and save it to log
    void showMessage(String string, MessageLevel level=MESSAGE_ERROR, bool hang=false, bool saveLog=true);
}