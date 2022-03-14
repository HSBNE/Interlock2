#ifndef InterlockCore
#define InterlockCore

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFi.h>

// Macros for build flags
#define ST(A...) #A
#define STR(A) ST(A)

enum State {
   IDLE, // Normal state of interlock (not being used)
   EXIT_IDLE, // Exiting the idle state and deciding which state to go into next. Allows a prompt to be sent to the user to say the card has been read.
   LOADING, // State while the device is booting up
   ACCESS_GRANTED, // Device will allow asccess until the state changes. E.g. an interlock keeping a tool on
   EXIT_ACCESS_GRANTED, // Exiting the ACCESS_GRANTED state. Allows a prompt to be sent to the user.
   ACCESS_DENIED, // Device will display access denied messages and then revert to normal modes
   ACCESS_PULSE, // Device will allow access for a given timeframe. E.g. a door opening and then locking a few seconds later
   ERROR // Uh oh
};

enum DeviceType {
   DOOR_DEVICE = 0,
   INTERLOCK_DEVICE = 1
};

namespace Core {
// ===============================================================================
//                             Configurable Values
//     Values that are commented out should be set using build flags.
// ===============================================================================

// Secrets are kept in secrets.cpp

// INTERLOCK or DOOR
//#define INTERLOCK
//#define DOOR

// Wifi
inline const char* wifiSSID = WIFI_SSID;
inline const char* hostAddress = "https://portal.hsbne.org";

// Device
inline const char* deviceName = DEVICE_NAME;
inline const uint8_t relayPin = 12;
inline const uint8_t onboardLEDPin = 13;
inline const uint8_t indicatorLEDPin = 14;

// Door settings
inline const uint8_t openTime = 15; // Time (s) to keep the door unlocked when a swipe occurs

// LEDs
inline const uint8_t numberOfLEDs = N_LEDS;

// LED RGBW order (one or the other)
//#define GRBW  // Usually on newer interlocks
//#define RGBW // Usually on older interlocks

// RFID
inline const uint8_t rfidIgnoreTime = 4; // How long (s) to ignore RFID reads after one has been read.
inline const uint8_t rfidLockoutTime = 5; // How long (s) to prevent a user 'tapping off' a machine after they have 'tapped on and vice versa.

// Change to the correct reader for the device
//#define OLD_READER
//#define RF125PS_READER // (new reader)

// Server
inline const uint16_t serverUpdateRate = 60; // How long (s) between server checkins

// ===============================================================================
//                           End Of Configurable Values
// ===============================================================================

# ifdef DOOR
inline const char* deviceTypeStr = "door";
inline const bool normallyOpen = false;
inline DeviceType deviceType = DeviceType::DOOR_DEVICE;
# endif
# ifdef INTERLOCK
inline const char* deviceTypeStr = "interlock";
inline const bool normallyOpen = true;
inline DeviceType deviceType = DeviceType::INTERLOCK_DEVICE;
# endif

// State
extern State currentState;

}

void log(String message);

String httpGET(String url);

#endif