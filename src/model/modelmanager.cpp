#include "modelmanager.hpp"

ModelManager::ModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : m_loadAndStoreStrategy(loadAndStoreStrategy) {
    qRegisterMetaType<ModelManager::State>("ModelManager::State");
    //qRegisterMetaType<ModelManager::Error>("ModelManager::Error");
}

ModelManager::~ModelManager() {
}
