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
    settings.setValue("correspondencesPath", preferences->getCorrespondencesPath());
    settings.setValue("segmentationImageFilesSuffix", preferences->getSegmentationImageFilesSuffix());
    settings.setValue("imageFilesExtension", preferences->getImageFilesExtension());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup(identifier + "-colorcodes");
    settings.remove("");
    for (auto objectModelIdentifier : preferences->getSegmentationCodes().keys()) {
        settings.setValue(objectModelIdentifier, preferences->getSegmentationCodeForObjectModel(objectModelIdentifier));
    }
    settings.endGroup();

    emit preferencesChanged(preferences->getIdentifier());
}

UniquePointer<Preferences> PreferencesStore::loadPreferencesByIdentifier(const QString &identifier) {
    UniquePointer<Preferences> preferences(new Preferences(identifier));
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    const QString &fullIdentifier = "preferences-" + identifier;
    settings.beginGroup(fullIdentifier);
    preferences->setImagesPath(settings.value("imagesPath", QDir::homePath()).toString());
    preferences->setObjectModelsPath(settings.value("objectModelsPath", QDir::homePath()).toString());
    preferences->setCorrespondencesPath(settings.value("correspondencesPath", QDir::homePath()).toString());
    preferences->setImageFilesExtension(settings.value("imageFilesExtension", "").toString());
    preferences->setSegmentationImageFilesSuffix(settings.value("segmentationImageFilesSuffix", "").toString());
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
