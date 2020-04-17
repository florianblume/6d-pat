#include "settings.hpp"


Settings::Settings(QString identifier) : identifier(identifier) {
}

Settings::Settings(const Settings &preferences) {
    this->segmentationCodes = preferences.segmentationCodes;
    this->segmentationImagesPath = preferences.segmentationImagesPath;
    this->imagesPath = preferences.imagesPath;
    this->objectModelsPath = preferences.objectModelsPath;
    this->posesFilePath = preferences.posesFilePath;
    this->identifier = preferences.identifier;
}

Settings::~Settings() {
}

QString Settings::getSegmentationImagesPath() const {
    return segmentationImagesPath;
}

void Settings::setSegmentationImagePath(const QString &value) {
    segmentationImagesPath = value;
}

QString Settings::getImagesPath() const {
    return imagesPath;
}

void Settings::setImagesPath(const QString &value) {
    imagesPath = value;
}

QString Settings::getObjectModelsPath() const {
    return objectModelsPath;
}

void Settings::setObjectModelsPath(const QString &value) {
    objectModelsPath = value;
}

QString Settings::getPosesFilePath() const {
    return posesFilePath;
}

void Settings::setPosesFilePath(const QString &value) {
    posesFilePath = value;
}

QMap<QString, QString> Settings::getSegmentationCodes() const {
    return segmentationCodes;
}

void Settings::setSegmentationCodes(const QMap<QString, QString> codes) {
    segmentationCodes = std::move(codes);
}

void Settings::removeSegmentationCodeForObjectModel(const QString &identifier) {
    segmentationCodes.remove(identifier);
}

void Settings::setSegmentationCodeForObjectModel(const QString &identifier, const QString &code) {
    segmentationCodes[identifier] = code;
}

QString Settings::getSegmentationCodeForObjectModel(const QString &identifier) {
    if (segmentationCodes.contains(identifier))
        return segmentationCodes[identifier];
    else
        return "";
}

QString Settings::getIdentifier() {
    return identifier;
}

QString Settings::getTrainingScriptPath() const
{
    return trainingScriptPath;
}

void Settings::setTrainingScriptPath(const QString &value)
{
    trainingScriptPath = value;
}

QString Settings::getInferenceScriptPath() const
{
    return inferenceScriptPath;
}

void Settings::setInferenceScriptPath(const QString &value)
{
    inferenceScriptPath = value;
}

QString Settings::getNetworkConfigPath() const
{
    return networkConfigPath;
}

void Settings::setNetworkConfigPath(const QString &value)
{
    networkConfigPath = value;
}

QString Settings::getPythonInterpreterPath() const
{
    return pythonInterpreterPath;
}

void Settings::setPythonInterpreterPath(const QString &value)
{
    pythonInterpreterPath = value;
}
