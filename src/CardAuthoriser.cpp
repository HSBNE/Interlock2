#include "CardAuthoriser.h"

CardAuthoriser::CardAuthoriser() { }

void CardAuthoriser::begin() {
    CardAuthoriser::cache.begin();
}


bool CardAuthoriser::checkCard(long cardNumber) {
    log("Checking for authorization");

    // Check for the skeleton key
    if (cardNumber == Core::skeletonKeyCardNumber) {
        log("Skeleton key was used");
        return true;
    }
    
    bool accessGranted;

    // HTTP
    String payload = httpGET(String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/check/" + String(cardNumber) + "/?secret=" + String(Core::hostSecret));
    log("[Network card auth] HTTP Payload: " + payload);
    if (payload != "") {
        // Auth with network
        const size_t capacity = JSON_OBJECT_SIZE(4) + 90;
        DynamicJsonDocument jsonDoc(capacity);
        deserializeJson(jsonDoc, payload);
        accessGranted = jsonDoc["access"].as<String>() == "true";
        jsonDoc.clear();

    } else {
        // Use cache
         accessGranted = CardAuthoriser::cache.checkCacheForCard(cardNumber);
    }

    const String accessStr = (accessGranted) ? "granted" : "denied";
    log("Access " + accessStr +" to " + String(cardNumber) );
    return accessGranted;
}

void CardAuthoriser::update() {
    // Get HTTP checkin response
    String payload = httpGET(String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/checkin/?secret=" + String(Core::hostSecret));
    log("[Update] HTTP Payload: " + payload);
    // Check for success
    if (payload == "") {
        return;
    }

    // Parse JSON
    const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
    DynamicJsonDocument jsonDoc(capacity);
    deserializeJson(jsonDoc, payload);
    String serverHash = jsonDoc["hashOfTags"].as<String>();

    // Check if the cache needs updating an do so if required.
    if (serverHash != CardAuthoriser::cache.getHash()) {
        cache.updateCacheFile();
    }
    jsonDoc.clear();

}

