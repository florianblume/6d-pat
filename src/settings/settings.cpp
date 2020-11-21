#include "settings.hpp"


Settings::Settings(const QString &identifier) : m_identifier(identifier) {
}

Settings::Settings(const Settings &preferences) {
    this->m_segmentationCodes = preferences.m_segmentationCodes;
    this->m_segmentationImagesPath = preferences.m_segmentationImagesPath;
    this->m_imagesPath = preferences.m_imagesPath;
    this->m_objectModelsPath = preferences.m_objectModelsPath;
    this->m_posesFilePath = preferences.m_posesFilePath;
    this->m_identifier = preferences.m_identifier;
}

Settings::~Settings() {
}

QString Settings::segmentationImagesPath() const {
    return m_segmentationImagesPath;
}

void Settings::setSegmentationImagePath(const QString &value) {
    m_segmentationImagesPath = value;
}

QString Settings::imagesPath() const {
    return m_imagesPath;
}

void Settings::setImagesPath(const QString &value) {
    m_imagesPath = value;
}

QString Settings::objectModelsPath() const {
    return m_objectModelsPath;
}

void Settings::setObjectModelsPath(const QString &value) {
    m_objectModelsPath = value;
}

QString Settings::posesFilePath() const {
    return m_posesFilePath;
}

void Settings::setPosesFilePath(const QString &value) {
    m_posesFilePath = value;
}

QMap<QString, QString> Settings::segmentationCodes() const {
    return m_segmentationCodes;
}

void Settings::setSegmentationCodes(const QMap<QString, QString> codes) {
    m_segmentationCodes = std::move(codes);
}

void Settings::removeSegmentationCodeForObjectModel(const QString &identifier) {
    m_segmentationCodes.remove(identifier);
}

void Settings::setSegmentationCodeForObjectModel(const QString &identifier, const QString &code) {
    m_segmentationCodes[identifier] = code;
}

QString Settings::segmentationCodeForObjectModel(const QString &identifier) const {
    if (m_segmentationCodes.contains(identifier))
        return m_segmentationCodes[identifier];
    else
        return "";
}

QString Settings::identifier() const {
    return m_identifier;
}

QString Settings::trainingScriptPath() const
{
    return m_trainingScriptPath;
}

void Settings::setTrainingScriptPath(const QString &value)
{
    m_trainingScriptPath = value;
}

QString Settings::inferenceScriptPath() const
{
    return m_inferenceScriptPath;
}

void Settings::setInferenceScriptPath(const QString &value)
{
    m_inferenceScriptPath = value;
}

QString Settings::networkConfigPath() const
{
    return m_networkConfigPath;
}

void Settings::setNetworkConfigPath(const QString &value)
{
    m_networkConfigPath = value;
}

QString Settings::pythonInterpreterPath() const
{
    return m_pythonInterpreterPath;
}

void Settings::setPythonInterpreterPath(const QString &value)
{
    m_pythonInterpreterPath = value;
}

Qt::MouseButton Settings::addCorrespondencePointMouseButton() const
{
    return m_addCorrespondencePointMouseButton;
}

void Settings::setAddCorrespondencePointMouseButton(
        const Qt::MouseButton &addCorrespondencePointMouseButton)
{
    m_addCorrespondencePointMouseButton = addCorrespondencePointMouseButton;
}

Qt::MouseButton Settings::moveBackgroundImageRenderableMouseButton() const
{
    return m_moveBackgroundImageRenderableMouseButton;
}

void Settings::setMoveBackgroundImageRenderableMouseButton(
        const Qt::MouseButton &moveBackgroundImageRenderableMouseButton)
{
    m_moveBackgroundImageRenderableMouseButton = moveBackgroundImageRenderableMouseButton;
}

Qt::MouseButton Settings::selectPoseRenderableMouseButton() const
{
    return m_selectPoseRenderableMouseButton;
}

void Settings::setSelectPoseRenderableMouseButton(
        const Qt::MouseButton &selectPoseRenderableMouseButton)
{
    m_selectPoseRenderableMouseButton = selectPoseRenderableMouseButton;
}

Qt::MouseButton Settings::rotatePoseRenderableMouseButton() const
{
    return m_rotatePoseRenderableMouseButton;
}

void Settings::setRotatePoseRenderableMouseButton(
        const Qt::MouseButton &rotatePoseRenderableMouseButton)
{
    m_rotatePoseRenderableMouseButton = rotatePoseRenderableMouseButton;
}

Qt::MouseButton Settings::translatePoseRenderableMouseButton() const
{
    return m_translatePoseRenderableMouseButton;
}

void Settings::setTranslatePoseRenderableMouseButton(
        const Qt::MouseButton &translatePoseRenderableMouseButton)
{
    m_translatePoseRenderableMouseButton = translatePoseRenderableMouseButton;
}
