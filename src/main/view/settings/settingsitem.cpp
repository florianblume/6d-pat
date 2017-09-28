#include "settingsitem.h"


SettingsItem::SettingsItem(QString identifier, ModelManager* modelManager) : identifier(identifier) {
    this->modelManager = modelManager;
}

SettingsItem::~SettingsItem() {
}

QString SettingsItem::getSegmentationImageFilesSuffix() const {
    return segmentationImageFilesSuffix;
}

void SettingsItem::setSegmentationImageFilesSuffix(const QString &value) {
    segmentationImageFilesSuffix = value;
}

QString SettingsItem::getImageFilesExtension() const {
    return imageFilesExtension;
}

void SettingsItem::setImageFilesExtension(const QString &value) {
    imageFilesExtension = value;
}

QString SettingsItem::getImagesPath() const {
    return imagesPath;
}

void SettingsItem::setImagesPath(const QString &value) {
    imagesPath = value;
}

QString SettingsItem::getObjectModelsPath() const {
    return objectModelsPath;
}

void SettingsItem::setObjectModelsPath(const QString &value) {
    objectModelsPath = value;
}

QString SettingsItem::getCorrespondencesPath() const {
    return correspondencesPath;
}

void SettingsItem::setCorrespondencesPath(const QString &value) {
    correspondencesPath = value;
}

void SettingsItem::getSegmentationCodes(QMap<const ObjectModel*, QString> &codes) const {
    QMap<const ObjectModel*, QString>::ConstIterator it(segmentationCodes.begin());
    for (; it != segmentationCodes.end(); it++) {
        codes[it.key()] = it.value();
    }
}

void SettingsItem::setSegmentationCodes(const QMap<const ObjectModel*, QString> codes) {
    segmentationCodes = codes;
}

void SettingsItem::setSegmentationCodeForObjectModel(const ObjectModel *objectModel, const QString &code) {
    segmentationCodes[objectModel] = code;
}

QString SettingsItem::getSegmentationCodeForObjectModel(const ObjectModel* objectModel) {
    return segmentationCodes[objectModel];
}
