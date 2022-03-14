#ifndef InterlockSecrets
#define InterlockSecrets

#include "Core.h"

namespace Core {
    // Set using build flags
    inline const char* wifiPassword = "";
    inline const char* otaPassword = STR(OTA_PASSWORD);
    inline const char* hostSecret = STR(HOST_SECRET); // Secret to talk to the host on
    inline const long skeletonKeyCardNumber = SKELETON_CARD; // This card number will allways grant access. (State = ACCESS_PULSE). Used to open doors in a worst case scenario.
}

#endif