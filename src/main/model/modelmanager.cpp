#include "modelmanager.hpp"

ModelManager::ModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : loadAndStoreStrategy(loadAndStoreStrategy) {
    loadAndStoreStrategy.addListener(this);
}

ModelManager::~ModelManager() {

}
