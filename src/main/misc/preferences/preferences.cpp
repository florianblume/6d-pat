#include "preferences.hpp"


Preferences::Preferences(QString identifier) : identifier(identifier) {
}

Preferences::Preferences(const Preferences &settingsItem) {
    this->segmentationCodes = settingsItem.segmentationCodes;
    this->segmentationImageFilesSuffix = settingsItem.segmentationImageFilesSuffix;
    this->imageFilesExtension = settingsItem.imageFilesExtension;
    this->imagesPath = settingsItem.imagesPath;
    this->objectModelsPath = settingsItem.objectModelsPath;
    this->correspondencesPath = settingsItem.correspondencesPath;
    this->identifier = settingsItem.identifier;
}

Preferences::~Preferences() {
}

QString Preferences::getSegmentationImageFilesSuffix() const {
    return segmentationImageFilesSuffix;
}

void Preferences::setSegmentationImageFilesSuffix(const QString &value) {
    segmentationImageFilesSuffix = value;
}

QString Preferences::getImageFilesExtension() const {
    return imageFilesExtension;
}

void Preferences::setImageFilesExtension(const QString &value) {
    imageFilesExtension = value;
}

QString Preferences::getImagesPath() const {
    return imagesPath;
}

void Preferences::setImagesPath(const QString &value) {
    imagesPath = value;
}

QString Preferences::getObjectModelsPath() const {
    return objectModelsPath;
}

void Preferences::setObjectModelsPath(const QString &value) {
    objectModelsPath = value;
}

QString Preferences::getCorrespondencesPath() const {
    return correspondencesPath;
}

void Preferences::setCorrespondencesPath(const QString &value) {
    correspondencesPath = value;
}

QMap<QString, QString> Preferences::getSegmentationCodes() const {
    return segmentationCodes;
}

void Preferences::setSegmentationCodes(const QMap<QString, QString> codes) {
    segmentationCodes = std::move(codes);
}

void Preferences::removeSegmentationCodeForObjectModel(const QString &identifier) {
    segmentationCodes.remove(identifier);
}

void Preferences::setSegmentationCodeForObjectModel(const QString &identifier, const QString &code) {
    segmentationCodes[identifier] = code;
}

QString Preferences::getSegmentationCodeForObjectModel(const QString &identifier) {
    if (segmentationCodes.contains(identifier))
        return segmentationCodes[identifier];
    else
        return "";
}

QString Preferences::getIdentifier() {
    return identifier;
}
