#include "Core.h"

namespace Core {
// ===============================================================================
//                             Configurable Values
// ===============================================================================

// Secrets are kept in secrets.cpp

// INTERLOCK or DOOR
DeviceType deviceType = DeviceType::DOOR;
#define DOOR

// Wifi
const char* wifiSSID = "HSBNEInfra";
const char* hostAddress = "https://portal.hsbne.org";

// Device
const char* deviceName = "DOOR-FRONT";
const uint8_t relayPin = 12;
const uint8_t onboardLEDPin = 13;
const uint8_t indicatorLEDPin = 14;

// Door settings
const uint8_t openTime = 15; // Time (s) to keep the door unlocked when a swipe occurs

// LEDs
const uint8_t numberOfLEDs = 16;
const neoPixelType ledType = NEO_GRBW + NEO_KHZ800; // Most interlocks are NEO_GRB, most doors are NEO_GRBW

// RFID
const uint32_t rfidIgnoreTime = 3; // How long (s) to ignore RFID reads after one has been read.

// Server
const uint16_t serverUpdateRate = 60; // How long (s) between server checkins

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

// Performs an HTTP GET request on the URL.
// Returns "" on error.
String httpGET(const String url) {

    // Do nothing if WiFi is disconnected.
    if (WiFi.status() != WL_CONNECTED) {
        return "";
    }

    // Do GET request
    std::unique_ptr<BearSSL::WiFiClientSecure>SSLClient(new BearSSL::WiFiClientSecure);
    SSLClient->setInsecure();
    HTTPClient httpClient;
    httpClient.begin(*SSLClient, url);
    int httpCode = httpClient.GET();
    log("HTTP Request: " + url);
    log("HTTP Code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
        log("HTTP Payload: " + httpClient.getString());
        return httpClient.getString();
    }

    return "";
}
