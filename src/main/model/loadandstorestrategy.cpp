#include "loadandstorestrategy.hpp"

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

bool LoadAndStoreStrategy::pathExists(const QDir &path) {
    QDir dir(path);
    return dir.exists();
}

void LoadAndStoreStrategy::addListener(LoadAndStoreStrategyListener* listener) {
    listeners.push_back(listener);
}

void LoadAndStoreStrategy::removeListener(LoadAndStoreStrategyListener* listener) {
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if(it != listeners.end())
        listeners.erase(it);
}
