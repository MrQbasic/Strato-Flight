#pragma once

namespace Sensors{

    struct Data{
        float temp_ext_C;
        
        float temp_ext_C_2;
        float pressure_ext;
        float humidity_ext;

        float uv_index;
    };

    extern Data data;

    //returns false on error which gets shown on display
    bool init();


    //do a singel readout of the sensors
    void update();
}