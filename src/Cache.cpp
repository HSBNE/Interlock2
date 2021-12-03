#include "Cache.h"

Cache::Cache() {}

void Cache::begin() {
    // Mount FS
    if (!LittleFS.begin()) {
        // TODO actually handle this error
        log("Failed to mount file system.");
        return;
    }
    log("Mounted file system");

    if (this->updateCacheFile()) {
        this->readCacheContentsToMemory();
    }
}

String Cache::getHash() {
    return Cache::hash;
}

// Updates cache from the server.
// Returns false on failure, true on success
bool Cache::updateCacheFile() {
    // Get payload and check for success
    String payload = httpGET(String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/authorised/?secret=" + String(Core::hostSecret));
    if (payload == "") {
        return false;
    }

    // Write to cache file
    File cacheFile = LittleFS.open("/cache.json", "w");
    if (!cacheFile) {
        log("Could not open cache file to write contents.");
        return false;
    }
    cacheFile.print(payload + '\n');
    cacheFile.close();
    log("Succesfully wrote cache file.");
    return true;
}

// Will read the cache content to memory if the current cache has is different from the server hash
// Returns false on failure, true on success
bool Cache::readCacheContentsToMemory() {
    // Clear the old cache
    std::memset(Cache::cacheArray, 0, sizeof(Cache::cacheArray));

    // Read cache file and store to buffer.
    File cacheFile = LittleFS.open("/cache.json", "r");
    if (!cacheFile) {
        log("Failed to open cache file.");;
        cacheFile.close();
        return false;
    }
    String cacheBuffer = cacheFile.readStringUntil('\n');
    cacheFile.close();

    // Parse JSON
    const size_t capacity = JSON_ARRAY_SIZE(200) + JSON_OBJECT_SIZE(2) + 1240;
    DynamicJsonDocument jsonDoc(capacity);
    deserializeJson(jsonDoc, cacheBuffer);

    // Update cache hash and load cards into memory
    Cache::hash = jsonDoc["authorised_tags_hash"].as<String>();
    JsonArray serverCards = jsonDoc["authorised_tags"];
    copyArray(serverCards, Cache::cacheArray);

    // Clean up
    jsonDoc.clear();

    // Set flag
    Cache::tagsLoadedInMemory = true;
    log("Loaded cards into memory.");
    return true;
}

bool Cache::checkCacheForCard(long cardNumber) {
    // Check that cache is loaded
    if (!Cache::tagsLoadedInMemory) {
        log("Tried to check cache when the cache was not laoded.");
        return false;
    }
    
    // Search cache
    for (uint16_t i = 0; i < sizeof(Cache::cacheArray); i++) {
        if (Cache::cacheArray[i] == cardNumber) {
            return true;
        }
    }
    return false;
}