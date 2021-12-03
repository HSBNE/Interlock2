#pragma once

#include "Core.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <mem.h>

class Cache {
    public:
        Cache();
        void begin();
        String getHash();
        bool checkCacheForCard(long cardNumber); // Returns true if the card was in the cache.
        bool updateCacheFile();
    private:
        String hash;
        bool tagsLoadedInMemory = false; // Prevents reloading the cache file if the tags are already stored in memory
        int cacheArray[200]; // Where the cards are stored in memory.
        bool readCacheContentsToMemory();
};