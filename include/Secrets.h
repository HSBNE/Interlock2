#ifndef InterlockSecrets
#define InterlockSecrets

namespace Core {
inline const char* wifiPassword = "";
inline const char* otaPassword = "";
inline const char* hostSecret = ""; // Secret to talk to the host on
inline const long skeletonKeyCardNumber = 0; // This card number will allways grant access. (State = ACCESS_PULSE). Used to open doors in a worst case scenario.
}

#endif