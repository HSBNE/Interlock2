#ifndef InterlockCore
#define InterlockCore

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFi.h>

enum State {
   IDLE, // Normal state of interlock (not being used)
   LOADING, // State while the device is booting up
   ACCESS_GRANTED, // Device will allow access until the state changes. E.g. an interlock keeping a tool on
   ACCESS_DENIED, // Device will display access denied messages and then revert to normal mode
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
// ===============================================================================

// Secrets are kept in secrets.cpp

// INTERLOCK or DOOR
//#define INTERLOCK
#define DOOR

// Wifi
inline const char* wifiSSID = "HSBNEInfra";
inline const char* hostAddress = "https://portal.hsbne.org";

// Device
inline const char* deviceName = "DOOR-Front";
inline const uint8_t relayPin = 12;
inline const uint8_t onboardLEDPin = 13;
inline const uint8_t indicatorLEDPin = 14;

// Door settings
inline const uint8_t openTime = 15; // Time (s) to keep the door unlocked when a swipe occurs

// LEDs
inline const uint8_t numberOfLEDs = 16;

// RFID
inline const uint32_t rfidIgnoreTime = 4; // How long (s) to ignore RFID reads after one has been read.

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