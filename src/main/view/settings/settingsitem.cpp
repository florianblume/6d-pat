#include "settingsitem.h"


SettingsItem::SettingsItem(QString identifier, ModelManager* modelManager) : identifier(identifier) {
    this->modelManager = modelManager;
}

SettingsItem::SettingsItem(const SettingsItem &settingsItem) {
    this->modelManager = settingsItem.modelManager;
    this->segmentationCodes = settingsItem.segmentationCodes;
    this->segmentationImageFilesSuffix = settingsItem.segmentationImageFilesSuffix;
    this->imageFilesExtension = settingsItem.imageFilesExtension;
    this->imagesPath = settingsItem.imagesPath;
    this->objectModelsPath = settingsItem.objectModelsPath;
    this->correspondencesPath = settingsItem.correspondencesPath;
    this->identifier = settingsItem.identifier;
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

QMap<QString, QString> SettingsItem::getSegmentationCodes() const {
    return segmentationCodes;
}

void SettingsItem::setSegmentationCodes(const QMap<QString, QString> codes) {
    segmentationCodes = std::move(codes);
}

void SettingsItem::removeSegmentationCodeForObjectModel(const QString &identifier) {
    segmentationCodes.remove(identifier);
}

void SettingsItem::setSegmentationCodeForObjectModel(const QString &identifier, const QString &code) {
    segmentationCodes[identifier] = code;
}

QString SettingsItem::getSegmentationCodeForObjectModel(const QString &identifier) {
    if (segmentationCodes.contains(identifier))
        return segmentationCodes[identifier];
    else
        return "";
}
