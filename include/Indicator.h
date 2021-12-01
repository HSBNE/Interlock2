#pragma once

#include "Core.h"

class Indicator {
    public:
        void update();
        Indicator();

    private:
        State lastInidcatorState;
        Adafruit_NeoPixel indicatorLEDs;
};