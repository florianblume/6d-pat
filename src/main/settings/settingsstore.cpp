#include "settingsstore.hpp"
#include <QSettings>
#include <QDir>

SettingsStore::SettingsStore()
{

}

QSharedPointer<Settings> SettingsStore::createEmptyPreferences(const QString &identifier) {
    return QSharedPointer<Settings>(new Settings(identifier));
}

void SettingsStore::savePreferences(Settings *settingsPointer) {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    const QString &identifier = "preferences-" + settingsPointer->getIdentifier();
    settings.beginGroup(identifier);
    settings.setValue("imagesPath", settingsPointer->getImagesPath());
    settings.setValue("objectModelsPath", settingsPointer->getObjectModelsPath());
    settings.setValue("posesFilePath", settingsPointer->getPosesFilePath());
    settings.setValue("segmentationImagesPath", settingsPointer->getSegmentationImagesPath());
    settings.setValue("pythonInterpreterPath", settingsPointer->getPythonInterpreterPath());
    settings.setValue("trainingScriptPath", settingsPointer->getTrainingScriptPath());
    settings.setValue("inferenceScriptPath", settingsPointer->getInferenceScriptPath());
    settings.setValue("networkConfigPath", settingsPointer->getNetworkConfigPath());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup(identifier + "-colorcodes");
    settings.remove("");
    for (auto objectModelIdentifier : settingsPointer->getSegmentationCodes().keys()) {
        settings.setValue(objectModelIdentifier, settingsPointer->getSegmentationCodeForObjectModel(objectModelIdentifier));
    }
    settings.endGroup();

    emit settingsChanged(settingsPointer->getIdentifier());
}

QSharedPointer<Settings> SettingsStore::loadPreferencesByIdentifier(const QString &identifier) {
    QSharedPointer<Settings> settingsPointer(new Settings(identifier));
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    const QString &fullIdentifier = "preferences-" + identifier;
    settings.beginGroup(fullIdentifier);
    settingsPointer->setImagesPath(
                settings.value("imagesPath", QDir::homePath()).toString());
    settingsPointer->setObjectModelsPath(
                settings.value("objectModelsPath", QDir::homePath()).toString());
    settingsPointer->setPosesFilePath(
                settings.value("posesFilePath", QDir::homePath()).toString());
    settingsPointer->setSegmentationImagePath(
                settings.value("segmentationImagesPath", "").toString());
    settingsPointer->setPythonInterpreterPath(
                settings.value("pythonInterpreterPath", "").toString());
    settingsPointer->setTrainingScriptPath(
                settings.value("trainingScriptPath", "").toString());
    settingsPointer->setInferenceScriptPath(
                settings.value("inferenceScriptPath", "").toString());
    settingsPointer->setNetworkConfigPath(
                settings.value("networkConfigPath", "").toString());
    settings.endGroup();

    settings.beginGroup(fullIdentifier + "-colorcodes");
    QStringList objectModelIdentifiers = settings.allKeys();
    for (const QString &objectModelIdentifier : objectModelIdentifiers) {
        const QString &colorCode = settings.value(objectModelIdentifier, "").toString();
        if (colorCode.compare("") != 0)
            settingsPointer->setSegmentationCodeForObjectModel(objectModelIdentifier, colorCode);
    }
    settings.endGroup();
    return settingsPointer;
}
