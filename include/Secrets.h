#ifndef InterlockSecrets
#define InterlockSecrets

#include "Core.h"

namespace Core {
    // Set using build flags
    inline const char* wifiPassword = WIFI_PASSWORD;
    inline const char* otaPassword = OTA_PASSWORD;
    inline const char* hostSecret = HOST_SECRET; // Secret to talk to the host on
    inline const long skeletonKeyCardNumber = SKELETON_CARD; // This card number will allways grant access. (State = ACCESS_PULSE). Used to open doors in a worst case scenario.
}

#endif