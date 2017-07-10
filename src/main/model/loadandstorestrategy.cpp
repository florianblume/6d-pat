#include "loadandstorestrategy.hpp"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

bool LoadAndStoreStrategy::pathExists(const path path) {
    return boost::filesystem::is_directory(path) || boost::filesystem::is_regular(path);
}

void LoadAndStoreStrategy::addListener(LoadAndStoreStrategyListener* listener) {
    listeners.push_back(listener);
}

void LoadAndStoreStrategy::removeListener(LoadAndStoreStrategyListener* listener) {
    listeners.remove(listener);
}
