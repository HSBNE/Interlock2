#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

enum State {
   IDLE, // Normal state of interlock (not being used)
   LOADING, // State while the device is booting up
   ACCESS_GRANTED, // Device will allow access until the state changes. E.g. an interlock keeping a tool on
   ACCESS_DENIED, // Device will display access denied messages and then revert to normal mode
   ACCESS_PULSE, // Device will allow access for a given timeframe. E.g. a door opening and then locking a few seconds later
   ERROR // Uh oh
};

enum DeviceType {
   DOOR = 0,
   INTERLOCK = 1
};

namespace Core {
// ===============================================================================
//                             Configurable Values
// ===============================================================================

// Secrets are kept in secrets.h

// Wifi
extern const char* wifiSSID;
extern const char* hostAddress;

// Device
extern const char* deviceName;
extern const uint8_t relayPin;
extern const uint8_t onboardLEDPin;
extern const uint8_t indicatorLEDPin;
extern DeviceType deviceType;

// Door settings
extern const uint8_t openTime;

// LEDs
extern const uint8_t numberOfLEDs;
   // TODO possible type error
extern const uint8_t ledType;


// RFID
extern const uint32_t rfidIgnoreTime;

// Server
extern const uint16_t serverUpdateRate;

extern const char* deviceTypeStr;
extern const bool normallyOpen;

// State
extern State currentState;

//Secrets
extern const char* wifiPassword;
extern const char* otaPassword;
extern const char* hostSecret;
extern const long skeletonKeyCardNumber;
}

void log(String message);

String httpGET(HTTPClient* httpClientPtr, String url);

