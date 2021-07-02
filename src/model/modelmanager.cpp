#include "modelmanager.hpp"

ModelManager::ModelManager(LoadAndStoreStrategyPtr loadAndStoreStrategy) : m_loadAndStoreStrategy(loadAndStoreStrategy) {
    qRegisterMetaType<ModelManager::State>("ModelManager::State");
}

ModelManager::~ModelManager() {
}
