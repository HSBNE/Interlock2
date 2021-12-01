#include "CardAuthoriser.h"
#include "Core.h"
#include "Indicator.h"
#include "RFIDReader.h"
#include "ExternalCommunicator.h"

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

// Objects
RFIDReader rfidReader;
HTTPClient httpClient;
Indicator indicator;
ExternalCommunicator external;
CardAuthoriser cardAuthoriser(&httpClient);

void log(String message) {
    Serial.println(message);
    external.broadcast(message);
}

// Setup and connect to WiFi
inline void setupWifI() {
    Serial.println("Starting WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(Core::wifiSSID, Core::wifiPassword);
    WiFi.hostname(Core::deviceName);
    // Block while WiFi connects
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.println("...");
    }
    log("WiFi Connected");
    log(WiFi.localIP().toString());
}

// Changes the state of the relay pin taking into account of the device's idle state is
// normally open or normally closed.
// Params:
// accessGranted should be set to true when you want the device to 'do the thing'.
//    true: open door / activate interlock
//    false lock door / turn off interlock.
void setRelay(bool accessGranted) {
    if (Core::normallyOpen) {
        digitalWrite(Core::relayPin, LOW);
    } else {
        digitalWrite(Core::relayPin, HIGH);
    }
}

void setup() {
    // Disable watchdog (always a fantstic idea...)
    // ESP.wdtDisable();

    // Serial
    Serial.begin(9600);
    delay(500);

    // Onboard LED
    pinMode(Core::onboardLEDPin, OUTPUT);
    digitalWrite(Core::onboardLEDPin, LOW);  //Turn on onboard LED to show we're alive

    // Indicator
    indicator.update();

    // Relay
    pinMode(Core::relayPin, OUTPUT);
    setRelay(false);

    // WiFi
    setupWifI();

    // OTA
    ArduinoOTA.setHostname(Core::deviceName);
    ArduinoOTA.setPassword(Core::otaPassword);
    ArduinoOTA.onStart([]() {
        log("Start");
    });
    ArduinoOTA.onEnd([]() {
        log("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            log("Auth Failed. Incorrecto OTA password");
        else if (error == OTA_BEGIN_ERROR)
            log("Begin Failed. Check firmware size.");
        else if (error == OTA_CONNECT_ERROR)
            log("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
            log("Receive Failed");
        else if (error == OTA_END_ERROR)
            log("End Failed");
    });
    ArduinoOTA.begin();

    // Websockets

    // Exit BOOTING State
    Core::currentState = State::IDLE;
    log("Finished booting");
}

// Idle.
// The device will normally be in this mode (locked door / unpowered interlock)
// * In this state we check to see if a card can be read and then take action.
// * On a successful read we will go into ACCESS_PULSE for a door or ACCESS_GRANTED for an
//   interlock.
// * On a bad read we go into ACCESS_DENIED
inline void idleAction() {
    // Ensure the relay is in the correct position.
    setRelay(false);

    // Check for available card Data
    if (Serial.available()) {
        log("Checking serial buffer"); // TODO remove
        long cardNumber = rfidReader.readCard();

        // Check that the read was valid / the reader did not encounter an error
        if (cardNumber < 0) return;

        // Check if card is authorised
        if (!cardAuthoriser.checkCard(cardNumber)) {
            Core::currentState = State::ACCESS_DENIED;
            return;
        }

// Pulse access for doors and grant access for interlocks
        if (Core::deviceType == DeviceType::DOOR) {
            Core::currentState = State::ACCESS_PULSE;
        } else if (Core::deviceType == DeviceType::INTERLOCK) {
            Core::currentState = State::ACCESS_GRANTED;
        }
        

        // Update LED indicator color
        indicator.update();
    }
}

// Access Granted
// Interlocks which are powered on use this mode. It will keep power to the relay
// until it is swiped off.
// * In this state we check for any card read.
// * The power to the relay is kept on.
// * If any card read occurs, authourized or not, we will go into idle mode and cut power.
inline void accessGrantedAction() {
    // Turn on relay
    digitalWrite(Core::relayPin, HIGH);

    // Check for valid card (authourized and unauthourized) reads. If there is one, change state to IDLE
    if (Serial.available()) {
        if (rfidReader.readCard() > 0) {
            Core::currentState = State::IDLE;
        }
    }

    indicator.update();
}

// Access Pulse
// Doors will be in the mode when they are unlocked. They will stay in this mode for the specified time in
// Core.h.
// * In this state we unlock the door and wait for time to pass.
// * When the time is up we lock the door and return to idle mode.
inline void accessPulseAction() {
    indicator.update();
    setRelay(true);
    delay(Core::openTime * 1000);
    setRelay(false);
    indicator.update();
    Core::currentState = State::IDLE;
}

// Access Denied
// This action happens when an unauthorised user tries to use the device.
// * The indicator will display access denied for 2 seconds.
// * The relay will be chnaged to access denied.
// * The device will return to idle mode.
inline void accessDeniedAction() {
    setRelay(false);
    indicator.update();
    delay(2000);
    Core::currentState = State::IDLE;
}

inline void errorAction() {
}

// State global. Everything runs off this.

void loop() {

    static State lastState;

    if (lastState != Core::currentState) {
        log("Current state: " + String(Core::currentState));
        lastState = Core::currentState;
    }

    // Do state machine
    switch (Core::currentState) {
        case State::IDLE:
            idleAction();
            break;

        case State::ACCESS_GRANTED:
            accessGrantedAction();
            break;

        case State::ACCESS_PULSE:
            accessPulseAction();
            break;

        case State::ACCESS_DENIED:
            accessDeniedAction();
            break;

        default: // Error.
            errorAction();
            break;
    }

    // Check if the state needs to change due to an external trigger (e.g. HTTP)
    Core::currentState = external.checkForStateChange();

    // Update the indicator for no reason other than a warm fuzzy feeling.
    indicator.update();

    // Flush the serial buffer
    rfidReader.flush();

    // Loop server
    external.loop();

    // OTA
    ArduinoOTA.handle();

    // Delay
    delay(500);
}
