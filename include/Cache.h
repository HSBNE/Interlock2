#pragma once

#include "Core.h"
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>

class Cache {
    public:
        Cache(HTTPClient *client);
        void update();
        bool checkCacheForCard(long cardNumber); // Returns true if the card was in the cache.
    private:
        String hash;
        bool tagsLoadedInMemory = false; // Prevents reloading the cache file if the tags are already stored in memory
        int cacheArray[200]; // Where the cards are stored in memory.
        HTTPClient *httpClientPtr;
};