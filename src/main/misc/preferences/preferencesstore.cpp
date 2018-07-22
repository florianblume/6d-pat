#include "preferencesstore.hpp"
#include <QSettings>
#include <QDir>

PreferencesStore::PreferencesStore()
{

}

UniquePointer<Preferences> PreferencesStore::createEmptyPreferences(const QString &identifier) {
    return std::move(UniquePointer<Preferences>(new Preferences(identifier)));
}

void PreferencesStore::savePreferences(Preferences *preferences) {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    const QString &identifier = "preferences-" + preferences->getIdentifier();
    settings.beginGroup(identifier);
    settings.setValue("imagesPath", preferences->getImagesPath());
    settings.setValue("objectModelsPath", preferences->getObjectModelsPath());
    settings.setValue("posesFilePath", preferences->getPosesFilePath());
    settings.setValue("segmentationImagesPath", preferences->getSegmentationImagesPath());
    settings.setValue("pythonInterpreterPath", preferences->getPythonInterpreterPath());
    settings.setValue("trainingScriptPath", preferences->getTrainingScriptPath());
    settings.setValue("inferenceScriptPath", preferences->getInferenceScriptPath());
    settings.setValue("networkConfigPath", preferences->getNetworkConfigPath());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup(identifier + "-colorcodes");
    settings.remove("");
    for (auto objectModelIdentifier : preferences->getSegmentationCodes().keys()) {
        settings.setValue(objectModelIdentifier, preferences->getSegmentationCodeForObjectModel(objectModelIdentifier));
    }
    settings.endGroup();

    Q_EMIT preferencesChanged(preferences->getIdentifier());
}

UniquePointer<Preferences> PreferencesStore::loadPreferencesByIdentifier(const QString &identifier) {
    UniquePointer<Preferences> preferences(new Preferences(identifier));
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    const QString &fullIdentifier = "preferences-" + identifier;
    settings.beginGroup(fullIdentifier);
    preferences->setImagesPath(
                settings.value("imagesPath", QDir::homePath()).toString());
    preferences->setObjectModelsPath(
                settings.value("objectModelsPath", QDir::homePath()).toString());
    preferences->setPosesFilePath(
                settings.value("posesFilePath", QDir::homePath()).toString());
    preferences->setSegmentationImagePath(
                settings.value("segmentationImagesPath", "").toString());
    preferences->setPythonInterpreterPath(
                settings.value("pythonInterpreterPath", "").toString());
    preferences->setTrainingScriptPath(
                settings.value("trainingScriptPath", "").toString());
    preferences->setInferenceScriptPath(
                settings.value("inferenceScriptPath", "").toString());
    preferences->setNetworkConfigPath(
                settings.value("networkConfigPath", "").toString());
    settings.endGroup();

    settings.beginGroup(fullIdentifier + "-colorcodes");
    QStringList objectModelIdentifiers = settings.allKeys();
    for (const QString &objectModelIdentifier : objectModelIdentifiers) {
        const QString &colorCode = settings.value(objectModelIdentifier, "").toString();
        if (colorCode.compare("") != 0)
            preferences->setSegmentationCodeForObjectModel(objectModelIdentifier, colorCode);
    }
    settings.endGroup();
    return preferences;
}
