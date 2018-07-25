#include "loadandstorestrategy.hpp"

LoadAndStoreStrategy::LoadAndStoreStrategy(SettingsStore *settingsStore,
                                           const QString &settingsIdentifier) :
    settingsStore(settingsStore),
    settingsIdentifier(settingsIdentifier) {
    if (settingsStore) {
        connect(settingsStore, &SettingsStore::settingsChanged,
                this, &LoadAndStoreStrategy::onSettingsChanged);
    }
}

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

void LoadAndStoreStrategy::setSettingsStore(SettingsStore *value) {
    if (settingsStore) {
        disconnect(settingsStore, &SettingsStore::settingsChanged,
                this, &LoadAndStoreStrategy::onSettingsChanged);
    }
    settingsStore = value;
    connect(settingsStore, &SettingsStore::settingsChanged,
            this, &LoadAndStoreStrategy::onSettingsChanged);
}

void LoadAndStoreStrategy::setSettingsIdentifier(const QString &value) {
    settingsIdentifier = value;
}
