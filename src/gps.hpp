#pragma once

#include "HT_TinyGPS++.h"

namespace GPS
{
    extern TinyGPSPlus GPS;

    void init();

    void updateData();

}
