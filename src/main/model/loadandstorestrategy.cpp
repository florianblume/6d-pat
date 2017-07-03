#include "loadandstorestrategy.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

bool LoadAndStoreStrategy::pathExists(const string path) {
    boost::filesystem::path _path(path);
    return boost::filesystem::is_directory(_path);
}

void LoadAndStoreStrategy::addListener(LoadAndStoreStrategyListener* listener) {
    listeners.push_back(listener);
}

void LoadAndStoreStrategy::removeListener(LoadAndStoreStrategyListener* listener) {
    listeners.remove(listener);
}
