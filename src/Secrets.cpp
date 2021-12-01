#include "Core.h"

namespace Core {
const char* wifiPassword = "";
const char* otaPassword = "";
const char* hostSecret = ""; // Secret to talk to the host on
const long skeletonKeyCardNumber = 0; // This card number will allways grant access. (State = ACCESS_PULSE). Used to open doors in a worst case scenario.
}
