// The human interface of the the interlock. Implmented as RGB LEDs

#include "Indicator.h"

void Indicator::begin() { // 
    this->led.Begin();
    this->led.Show();    
}

void Indicator::update() { // TODO Using 16 LEDs triggers Exception 28 (Access to invalid address)
    // Do nothing if the state has not changed. Otherwise update the last state
    if (this->lastInidcatorState == Core::currentState) {
        return;
    }
    this->lastInidcatorState = Core::currentState;
    

    // Set the color of the LEDs
    // Uses RGBW color values
    RgbwColor color;
    switch (Core::currentState) {
        case State::LOADING:
            // Purple
            color = RgbwColor(255, 0, 255, 0);
            break;
        
        case State::IDLE:
            // Blue
            color = RgbwColor(0, 0, 255, 0);
            break;

        case State::EXIT_IDLE:
            // Purple
            color = RgbwColor(255, 0, 255, 0);
            break;

        case State::ACCESS_GRANTED:
            // Green
            color = RgbwColor(0, 255, 0, 0);
            break;

        case State::EXIT_ACCESS_GRANTED:
            // Purple
            color = RgbwColor(255, 0, 255, 0);
            break;

        case State::ACCESS_PULSE:
            // Green
            color = RgbwColor(0, 255, 0, 0);
            break;

        case State::ACCESS_DENIED:
            // Red
            color = RgbwColor(255, 0, 0, 0);
            break;
        default:
            // Default is ERROR
            // Yellow
            color = RgbwColor(255, 255, 0, 0);
            break;
    }

    for(uint16_t i = 0; i < Core::numberOfLEDs; i++) {
        led.SetPixelColor(i, color);
    }
    this->led.Show();
}