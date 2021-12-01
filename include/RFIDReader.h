#pragma once

#include "Core.h"

class RFIDReader {
    public:
        long readCard();
        void flush();
    private:
        unsigned long lastReadTime = 0;
};