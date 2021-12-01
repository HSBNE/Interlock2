#include "Core.h"

namespace Core {
// ===============================================================================
//                             Configurable Values
// ===============================================================================

// Secrets are kept in secrets.h

// INTERLOCK or DOOR
DeviceType deviceType = DeviceType::DOOR;
#define DOOR

// Wifi
const char* wifiSSID = "FBIVan";
const char* hostAddress = "http://portal.hsbne.org";

// Device
const char* deviceName = "DOOR-Front";
const uint8_t relayPin = 12;
const uint8_t onboardLEDPin = 13;
const uint8_t indicatorLEDPin = 14;

// Door settings
const uint8_t openTime = 15; // Time (s) to keep the door unlocked when a swipe occurs

// LEDs
const uint8_t numberOfLEDs = 16;
const uint8_t ledType = NEO_BGRW + NEO_KHZ800; // Most interlocks are GRB, most doors are BGRW // TODO possible type error


// RFID
const uint32_t rfidIgnoreTime = 5; // How long (s) to ignore RFID reads after one has been read.

// Server
const uint16_t checkinRate = 60; // How long (s) between server checkins
const uint16_t sessionCheckinRate = 60; // How long (s) between intelrock checkins

// ===============================================================================
//                           End Of Configurable Values
// ===============================================================================

#ifdef DOOR
const char* deviceTypeStr = "door";
const bool normallyOpen = false;
#endif
#ifdef INTERLOCK
const char* deviceTypeStr = "interlock";
const bool normallyOpen = true;
#endif

// State
State currentState = State::LOADING;

}

