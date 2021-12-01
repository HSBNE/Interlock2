#pragma once

class RFIDReader {
    public:
        long readCard();
        void flush();
    private:
        unsigned long lastReadTime = 0;
};