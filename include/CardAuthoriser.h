// This class is used do determine if a card is valid or not.
// Internally, it handles if the the device is in networked of cached mode.

#pragma once

#include "Core.h"
#include "ExternalCommunicator.h"
#include "Cache.h"
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

class CardAuthoriser {
    public:
        CardAuthoriser(HTTPClient *client);
        bool checkCard(long cardNumber); // True if the card is authroised. False otherwise.
        void checkIn(); // Talks to the serve to update cache.
        

    private:
        Cache cache;
        HTTPClient *httpClientPtr;

};