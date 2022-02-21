#pragma once

#include "Core.h"
#include <NeoPixelBus.h>

class Indicator {
    public:
        void update();
        void begin();

    private:
        State lastInidcatorState = State::ERROR;
        #ifdef GRBW
        NeoPixelBus<NeoGrbwFeature, NeoEsp8266BitBang800KbpsMethod> led = NeoPixelBus<NeoGrbwFeature, NeoEsp8266BitBang800KbpsMethod>(Core::numberOfLEDs, Core::indicatorLEDPin);
        #endif
        #ifdef RGBW
        NeoPixelBus<NeoRgbwFeature, NeoEsp8266BitBang800KbpsMethod> led = NeoPixelBus<NeoRgbwFeature, NeoEsp8266BitBang800KbpsMethod>(Core::numberOfLEDs, Core::indicatorLEDPin);
        #endif
};