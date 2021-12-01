#pragma once

#include "Core.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <mem.h>

class Cache {
    public:
        Cache(HTTPClient *client);
        String getHash();
        void updateCacheFile();
        bool checkCacheForCard(long cardNumber); // Returns true if the card was in the cache.
    private:
        String hash;
        bool tagsLoadedInMemory = false; // Prevents reloading the cache file if the tags are already stored in memory
        int cacheArray[200]; // Where the cards are stored in memory.
        HTTPClient *httpClientPtr;
        inline void readCacheContentsToMemory();
};