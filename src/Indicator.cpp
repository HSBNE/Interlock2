// The human interface of the the interlock. Implmented as RGB LEDs

#include "Indicator.h"

void Indicator::begin() { // 
    this->indicatorLEDs = Adafruit_NeoPixel(Core::numberOfLEDs, Core::indicatorLEDPin, Core::ledType);
    this->indicatorLEDs.begin();
    this->update();
}

void Indicator::update() { // TODO Using 16 LEDs triggers Exception 28 (Access to invalid address)
    // Do nothing if the state has not changed. Otherwise update the last state
    if (this->lastInidcatorState == Core::currentState) {
        return;
    }
    this->lastInidcatorState = Core::currentState;
    

    // Set the color of the LEDs
    // Uses RGBW color values
    uint32_t color;
    switch (Core::currentState) {
        case State::LOADING:
            // Purple
            color =  this->indicatorLEDs.Color(255, 0, 255, 0);
            break;
        
        case State::IDLE:
            // Blue
            color =  this->indicatorLEDs.Color(0, 0, 255, 0);
            break;

        case State::ACCESS_GRANTED:
            // Green
            color =  this->indicatorLEDs.Color(0, 255, 0, 0);
            break;

        case State::ACCESS_PULSE:
            // Green
            color =  this->indicatorLEDs.Color(0, 255, 0, 0);
            break;

        case State::ACCESS_DENIED:
            // Red
            color =  this->indicatorLEDs.Color(255, 0, 0, 0);
            break;
        default:
            // Default is ERROR
            // Yellow
            color =  this->indicatorLEDs.Color(255, 255, 0, 0);
            break;
    }

    log("COLOR = " + String(color));
    this->indicatorLEDs.fill(color, 0, 0);
    this->indicatorLEDs.show();
}