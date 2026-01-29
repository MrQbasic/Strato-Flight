#pragma once
#include <SPI.h>

namespace Storage {

    //returns storage space info in MB
    void getSpaceInfo(int *totalMB, int *usedMB);
    
    void init_storage(SPIClass *spiSD);

    void saveData();

    void saveLogLine(String line);
    
}