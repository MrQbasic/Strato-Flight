#pragma once

namespace Storage {

    //returns storage space info in MB
    void getSpaceInfo(int *totalMB, int *usedMB);
    
    void init();

    void saveData();

    void saveLogLine(String line);
    
}