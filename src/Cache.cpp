#include "Cache.h"

Cache::Cache(HTTPClient *client) : httpClientPtr(client) {

    // TODO Setup
    this->update();
}

void Cache::update() {
    // TODO

}

bool Cache::checkCacheForCard(long cardNumber) {
    // TODO
    return false;
}