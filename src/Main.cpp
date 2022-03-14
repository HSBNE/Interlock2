#include <ArduinoOTA.h>

#include "CardAuthoriser.h"
#include "Core.h"
#include "Secrets.h"
#include "ExternalCommunicator.h"
#include "Indicator.h"
#include "RFIDReader.h"

// Objects
RFIDReader rfidReader;
Indicator indicator;
CardAuthoriser cardAuthoriser;
ExternalCommunicator external(&cardAuthoriser);  // External communicator is a misleading name.

// TODO it would be nice to move this so it's implemented in Core.cpp In the current scheme Core.h is implmented in two files (here and Core.cpp)
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
    if (accessGranted) {
        if (Core::normallyOpen) {
            digitalWrite(Core::relayPin, HIGH);
        } else {
            digitalWrite(Core::relayPin, LOW);
        }
    } else {
        if (Core::normallyOpen) {
            digitalWrite(Core::relayPin, LOW);
        } else {
            digitalWrite(Core::relayPin, HIGH);
        }
    }
}

void setup() {
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
    ArduinoOTA.setPort(8266);
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
            log("Auth Failed. Incorrect OTA password");
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

    // Card Authoriser and Cache
    cardAuthoriser.begin();

    // Exit BOOTING State
    Core::currentState = State::IDLE;
    log("Finished booting");
}

// Idle.
// The device will normally be in this mode (locked door / unpowered interlock)
// * In this state we check to see if a card can be read. If it can, we enmter EXIT_IDLE
//   This read will only occur after a delay. This prevents people accidentally 'tapping off' and then 'tapping back on' right away.
inline void idleAction(unsigned long lastTimeStateChanged) {
    // Ensure the relay is in the correct position.
    setRelay(false);

    // Check for available card Data
    if (Serial.available() && ((millis() - lastTimeStateChanged) / 1000) > Core::rfidLockoutTime) {
        Core::currentState = State::EXIT_IDLE;
    }
    indicator.update();
}

// Exit Idle
// * Read the RFID card
// 
// * On a successful read we will go into ACCESS_PULSE for a door or ACCESS_GRANTED for an
//   interlock.
// * On a bad read we go into ACCESS_DENIED
inline void exitIdleAction() {
    indicator.update();
    long cardNumber = rfidReader.readCard();

    // Check that the read was valid / the reader did not encounter an error
    if (cardNumber < 0) {
        Core::currentState = State::IDLE;
        return;
    }

    // Check if card is authorised
    if (!cardAuthoriser.checkCard(cardNumber)) {
        Core::currentState = State::ACCESS_DENIED;
        return;
    }

    // Pulse access for doors and grant access for interlocks
    if (Core::deviceType == DeviceType::DOOR_DEVICE) {
        Core::currentState = State::ACCESS_PULSE;
    } else if (Core::deviceType == DeviceType::INTERLOCK_DEVICE) {
        Core::currentState = State::ACCESS_GRANTED;
    }
}

// Access Granted
// Interlocks which are powered on use this mode. It will keep power to the relay
// until it is swiped off.
// * In this state we check for any card read.
// * The power to the relay is kept on.
// * After a delay, if any card read occurs, authourized or not, we will go into idle mode and cut power.
inline void accessGrantedAction(unsigned long lastTimeStateChanged) {
    // Turn on relay
    digitalWrite(Core::relayPin, HIGH);

    // Check for valid card (authourized and unauthourized) reads. If there is one, change state to IDLE
    // Only check for a valid card after a delay. This prevents people from accidentally turning on a machine and then turning it off right away.
    if (Serial.available() && ((millis() - lastTimeStateChanged) / 1000) > Core::rfidLockoutTime) {
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
    // TODO
    indicator.update();
}


void loop() {
    static State lastState;
    static unsigned long lastTime = 0; // Used to slow down request to the server
    static unsigned long lastTimeStateChanged = 0; // Used to determine how long we have been in a state.

    if (lastState != Core::currentState) {
        log("Current state: " + String(Core::currentState));
        lastState = Core::currentState;
        lastTimeStateChanged = millis();
    }

    // Do state machine
    switch (Core::currentState) {
        case State::IDLE:
            idleAction(lastTimeStateChanged);
            break;

        case State::EXIT_IDLE:
            exitIdleAction();
            break;

        case State::ACCESS_GRANTED:
            accessGrantedAction(lastTimeStateChanged);
            break;

        case State::ACCESS_PULSE:
            accessPulseAction();
            break;

        case State::ACCESS_DENIED:
            accessDeniedAction();
            break;

        default:  // Error.
            errorAction();
            break;
    }

    // Check if the state needs to change due to an external trigger (e.g. HTTP)
    Core::currentState = external.checkForStateChange();

    // Flush the serial buffer
    rfidReader.flush();

    // Loop server
    external.loop();

    // OTA
    ArduinoOTA.handle();

    // Server updates
    if ((millis() - lastTime) / 1000 > Core::serverUpdateRate) {
        lastTime = millis();
        cardAuthoriser.update();
    }

    // Delay
    delay(500);
}
