namespace Sensors{

    struct Data{
        float temp_ext_C;


    };

    extern Data data;

    //returns false on error which gets shown on display
    bool init();


    //do a singel readout of the sensors
    void update();
}