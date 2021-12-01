#include "Cache.h"

Cache::Cache(HTTPClient *client) : httpClientPtr(client) {

    // Mount FS
    if (!LittleFS.begin()) {
        // TODO actually handle this error
        log("Failed to mount file system.");
        return;
    }

    this->updateCacheFile();
    this->readCacheContentsToMemory();
}

String Cache::getHash() {
    return Cache::hash;
}

// Will read the cache content to memory if the current cache has is different from the server hash
inline void Cache::readCacheContentsToMemory() {
    // Clear the old cache
    std::memset(Cache::cacheArray, 0, sizeof(Cache::cacheArray));

    // Read cache file and store to buffer.
    File cacheFile = LittleFS.open("/cache", "r");
    if (cacheFile) {
        log("Failed to open cache file.");
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
}

// Updates cache from the server.
void Cache::updateCacheFile() {
    // Get payload and check for success
    String payload = httpGET(Cache::httpClientPtr, String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/authorised/?secret=" + String(Core::hostSecret));
    if (payload == "") {
        return;
    }

    // Write to cache file
    File cacheFile = LittleFS.open("/cache", "w");
    if (!cacheFile) {
        log("Could not open cache file to write contents.");
        return;
    }
    cacheFile.print(payload + '\n');
    cacheFile.close();

    // Update tags in memory and the hash
    this->readCacheContentsToMemory();
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