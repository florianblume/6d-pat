#include "modelmanager.h"

ModelManager::ModelManager(LoadAndStoreStrategy& _loadAndStoreStrategy) : loadAndStoreStrategy(&_loadAndStoreStrategy) {
    loadAndStoreStrategy->addListener(this);
}

ModelManager::~ModelManager() {

}
