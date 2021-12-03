// The human interface of the the interlock. Implmented as RGB LEDs

#include "Indicator.h"

Indicator::Indicator() {
    Indicator::indicatorLEDs = Adafruit_NeoPixel(Core::numberOfLEDs, Core::indicatorLEDPin, Core::ledType);
    Indicator::lastInidcatorState = Core::currentState;
    Indicator::indicatorLEDs.begin();
}

void Indicator::update() { // TODO Using 16 LEDs triggers Exception 28 (Access to invalid address)
    // Do nothing if the state has not changed. Otherwise update the last state/
    if (Indicator::lastInidcatorState == Core::currentState) {
        return;
    } else {
        Indicator::lastInidcatorState = Core::currentState;
    }

    // Set the color of the LEDs
    // Uses RGBW color values
    uint32_t color;
    switch (Core::currentState) {
        case State::LOADING:
            // Purple
            color = Indicator::indicatorLEDs.Color(255, 0, 255, 0);
            break;
        
        case State::IDLE:
            // Blue
            color = Indicator::indicatorLEDs.Color(0, 0, 255, 0);
            break;

        case State::ACCESS_GRANTED:
            // Green
            color = Indicator::indicatorLEDs.Color(0, 255, 0, 0);
            break;

        case State::ACCESS_PULSE:
            // Green
            color = Indicator::indicatorLEDs.Color(0, 255, 0, 0);
            break;

        case State::ACCESS_DENIED:
            // Red
            color = Indicator::indicatorLEDs.Color(255, 0, 0, 0);
            break;
        default:
            // Default is ERROR
            // Yellow
            color = Indicator::indicatorLEDs.Color(255, 255, 0, 0);
            break;
    }

    Indicator::indicatorLEDs.fill(color, 0, 0);
    Indicator::indicatorLEDs.show();
}