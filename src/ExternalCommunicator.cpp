#include "ExternalCommunicator.h"

// Initialize static members.
bool ExternalCommunicator::changeStateFlag = false;
State ExternalCommunicator::changeState = State::ERROR;
ESP8266WebServer* ExternalCommunicator::httpServerPtr = nullptr;
CardAuthoriser* ExternalCommunicator::cardAuthoriserPtr = nullptr;


ExternalCommunicator::ExternalCommunicator(CardAuthoriser* cardAuthoriser) {
    // Assign server ptrs. It's used to allow the static HTTP methods access to the webserver.
    ExternalCommunicator::httpServerPtr = &(ExternalCommunicator::httpServer);
    ExternalCommunicator::cardAuthoriserPtr = cardAuthoriser;

    // Setup websockets
    ExternalCommunicator::websocketServer.begin();

    // Setup HTTP
    ExternalCommunicator::httpServer.on("/",  ExternalCommunicator::httpRoot);
    ExternalCommunicator::httpServer.on("/bump",  ExternalCommunicator::httpBump);
    ExternalCommunicator::httpServer.on("/activate",  ExternalCommunicator::httpActivate);
    ExternalCommunicator::httpServer.on("/reboot",  ExternalCommunicator::httpReboot);
    ExternalCommunicator::httpServer.begin();
    log("Websocket and HTTP servers started.");

}

State ExternalCommunicator::checkForStateChange() {
    // Do nothing if the changeStateFlag is not set
    if (!ExternalCommunicator::changeStateFlag) {
        return Core::currentState;
    }

    // Unset the flag and return with the state that the device needs to enter.
    ExternalCommunicator::changeStateFlag = false;
    return ExternalCommunicator::changeState;
}

void ExternalCommunicator::broadcast(String message) {
    ExternalCommunicator::websocketServer.broadcastTXT(message);
}

void ExternalCommunicator::loop() {
    ExternalCommunicator::websocketServer.loop();
    ExternalCommunicator::httpServer.handleClient();
}

// Action to take on http://device.local/
void ExternalCommunicator::httpRoot() {
  static const String message = "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data); var logObj = document.getElementById('logs'); logObj.insertAdjacentHTML('afterend', e.data + '</br>');;};</script><title>" + String(Core::deviceName) + "</title></head><h1>" + String(Core::deviceName) + "</h1><h2>Logs: </h2> <div id ='logs'> </div> ";
  ExternalCommunicator::httpServerPtr->send(200, "text/html", message);
}

// Action to take on http://device.local/reboot
// Reboots device
void ExternalCommunicator::httpReboot() {
    log("Rebooting");
    ExternalCommunicator::httpRedirectToRoot();
    delay(100);
    ESP.reset();
}

// Action to take on http://device.local/activate?cardid=XXXX
// Turns on an interlock (ACCESS_GRANTED state) if cardid is valid
void ExternalCommunicator::httpActivate() {
    long cardId = (*ExternalCommunicator::httpServerPtr).arg("cardid").toInt();
    log("Activating interlock with " + String(cardId) + "via HTTP");
    if ((*ExternalCommunicator::cardAuthoriserPtr).checkCard(cardId)) {
        ExternalCommunicator::changeState = State::ACCESS_GRANTED;
        ExternalCommunicator::changeStateFlag = true;
        ExternalCommunicator::httpRedirectToRoot();
    }
}

// Action to take on http://device.local/bump
// Opens a door (ACCESS_PULSE state)
void ExternalCommunicator::httpBump() {
    log("Opening door");
    ExternalCommunicator::changeState = State::ACCESS_PULSE;
    ExternalCommunicator::changeStateFlag = true;
    ExternalCommunicator::httpRedirectToRoot();
}

void ExternalCommunicator::httpRedirectToRoot() {
    (*ExternalCommunicator::httpServerPtr).sendHeader("Location", "/");
    (*ExternalCommunicator::httpServerPtr).send(302, "text/plain", "");
}
