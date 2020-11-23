#include "modelmanager.hpp"

ModelManager::ModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : loadAndStoreStrategy(loadAndStoreStrategy) {
    qRegisterMetaType<ModelManager::State>("ModelManager::State");
}

ModelManager::~ModelManager() {
}
