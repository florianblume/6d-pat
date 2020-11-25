#include "loadandstorestrategy.hpp"

LoadAndStoreStrategy::LoadAndStoreStrategy() {
    qRegisterMetaType<LoadAndStoreStrategy::Error>("LoadAndStoreStrategy::Error");
}

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

void LoadAndStoreStrategy::setSettingsStore(SettingsStore *value) {
}

void LoadAndStoreStrategy::setSettingsIdentifier(const QString &value) {
}
