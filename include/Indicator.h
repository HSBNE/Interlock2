#pragma once

#include "Core.h"

class Indicator {
    public:
        void update();
        void begin();

    private:
        State lastInidcatorState = State::ERROR;
        Adafruit_NeoPixel indicatorLEDs;
};