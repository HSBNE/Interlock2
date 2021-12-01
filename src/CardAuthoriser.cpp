#include "CardAuthoriser.h"
#include "Core.h"

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

    // SSL
    String url = String(Core::hostAddress) + "/api/" + Core::deviceTypeStr + "/check/" + String(cardNumber) + "/?secret=" + String(Core::hostSecret);
    std::unique_ptr<BearSSL::WiFiClientSecure>SSLClient(new BearSSL::WiFiClientSecure);
    SSLClient->setInsecure();
    (*CardAuthoriser::httpClientPtr).begin(*SSLClient, url);

    // HTTP
    if ((*CardAuthoriser::httpClientPtr).GET() == HTTP_CODE_OK) {
        // Auth with network
        String payload = (*CardAuthoriser::httpClientPtr).getString();
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