#include "CardAuthoriser.h"

CardAuthoriser::CardAuthoriser(HTTPClient *client) : cache(client) {
    CardAuthoriser::httpClientPtr = client;
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
    String payload = httpGET(CardAuthoriser::httpClientPtr, String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/check/" + String(cardNumber) + "/?secret=" + String(Core::hostSecret));
    if (payload != "") {
        // Auth with network
        const size_t capacity = JSON_OBJECT_SIZE(4) + 90;
        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, payload);
        accessGranted = doc["access"].as<String>() == "true";
        doc.clear();

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
    String payload = httpGET(CardAuthoriser::httpClientPtr, String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/checkin/?secret=" + String(Core::hostSecret));

    // Check for success
    if (payload == "") {
        return;
    }

    // Parse JSON
    const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
    DynamicJsonDocument jsonDoc(capacity);
    String serverHash = jsonDoc["hashOfTags"].as<String>();

    // Check if the cache needs updating an do so if required.
    if (serverHash != cache.getHash()) {
        cache.updateCacheFile();
    }

}

