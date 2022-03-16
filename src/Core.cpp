#include "Core.h"
#include "Secrets.h"

namespace Core {
    State currentState = State::LOADING;
}

// Performs an HTTP GET request on the URL.
// Returns "" on error.
String httpGET(String url) {

    // Do nothing if WiFi is disconnected.
    if (WiFi.status() != WL_CONNECTED) {
        return "";
    }

    // Do GET request
    std::unique_ptr<BearSSL::WiFiClientSecure>SSLClient(new BearSSL::WiFiClientSecure);
    SSLClient->setInsecure();
    HTTPClient httpClient;
    httpClient.begin(*SSLClient, url);
    int httpCode = httpClient.GET();
    log("HTTP Request: " + url);
    log("HTTP Code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
        log("HTTP Payload: " + httpClient.getString());
        return httpClient.getString();
    }

    return "";
}
