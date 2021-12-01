// This class containerizes the  that the interlock displays its logs on
// and accepts commands from.

#pragma once

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include "Core.h"

class ExternalCommunicator {
   public:
    ExternalCommunicator();
    void loop();  // Loops the websocket and HTTP servers. To be called in the main loop.
    void broadcast(String message);  // Broadcasts logs

    // This will return the state that the device should be put into as a
    // function of external triggers (e.g. HTTP). If there are no external
    // triggers that changed the state of the device it will return the current
    // state of the device.
    State checkForStateChange();

   private:
    // Servers
    WebSocketsServer websocketServer = WebSocketsServer(81);
    ESP8266WebServer httpServer = ESP8266WebServer(80);
    static ESP8266WebServer* httpServerPtr; // Should probably be a reference but lazy.

    // HTTP;
    static void httpRoot(); // Action to take on http://device.local/
    static void httpReboot(); // Action to take on http://device.local/reboot
    static void httpActivate(); // Action to take on http://device.local/activate
    static void httpBump(); // Action to take on http://device.local/bump
    static void httpRedirectToRoot(); // Reditrect the client to http://device.local/

    // Internal state
    static bool changeStateFlag;  // This keeps track of if an external action (e.g. HTTP) requiring a change in the device's state.
    static State changeState;  // The state to change into if changeStateFlag is set.
};