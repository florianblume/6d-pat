#include "settings.hpp"

const QMap<Qt::MouseButton, int> Settings::MOUSE_BUTTONS = {{Qt::LeftButton,    0},
                                                            {Qt::RightButton,   1},
                                                            {Qt::MiddleButton,  2},
                                                            {Qt::BackButton,    3}};


Settings::Settings(const QString &identifier) : m_identifier(identifier) {
}

Settings::Settings(const Settings &settings) {
    this->m_segmentationCodes = settings.m_segmentationCodes;
    this->m_segmentationImagesPath = settings.m_segmentationImagesPath;
    this->m_imagesPath = settings.m_imagesPath;
    this->m_objectModelsPath = settings.m_objectModelsPath;
    this->m_posesFilePath = settings.m_posesFilePath;
    this->m_identifier = settings.m_identifier;
    this->m_pythonInterpreterPath = settings.m_pythonInterpreterPath;
    this->m_loadSaveScriptPath = settings.m_loadSaveScriptPath;
    this->m_usedLoadAndStoreStrategy = settings.m_usedLoadAndStoreStrategy;
    this->m_addCorrespondencePointMouseButton = settings.m_addCorrespondencePointMouseButton;
    this->m_moveBackgroundImageRenderableMouseButton = settings.m_moveBackgroundImageRenderableMouseButton;
    this->m_selectPoseRenderableMouseButton = settings.m_selectPoseRenderableMouseButton;
    this->m_translatePoseRenderableMouseButton = settings.m_translatePoseRenderableMouseButton;
    this->m_rotatePoseRenderableMouseButton = settings.m_rotatePoseRenderableMouseButton;
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

Qt::MouseButton Settings::addCorrespondencePointMouseButton() const {
    return m_addCorrespondencePointMouseButton;
}

void Settings::setAddCorrespondencePointMouseButton(
        const Qt::MouseButton &addCorrespondencePointMouseButton) {
    m_addCorrespondencePointMouseButton = addCorrespondencePointMouseButton;
}

Qt::MouseButton Settings::moveBackgroundImageRenderableMouseButton() const {
    return m_moveBackgroundImageRenderableMouseButton;
}

void Settings::setMoveBackgroundImageRenderableMouseButton(
        const Qt::MouseButton &moveBackgroundImageRenderableMouseButton) {
    m_moveBackgroundImageRenderableMouseButton = moveBackgroundImageRenderableMouseButton;
}

Qt::MouseButton Settings::selectPoseRenderableMouseButton() const {
    return m_selectPoseRenderableMouseButton;
}

void Settings::setSelectPoseRenderableMouseButton(
        const Qt::MouseButton &selectPoseRenderableMouseButton) {
    m_selectPoseRenderableMouseButton = selectPoseRenderableMouseButton;
}

Qt::MouseButton Settings::rotatePoseRenderableMouseButton() const {
    return m_rotatePoseRenderableMouseButton;
}

void Settings::setRotatePoseRenderableMouseButton(
        const Qt::MouseButton &rotatePoseRenderableMouseButton) {
    m_rotatePoseRenderableMouseButton = rotatePoseRenderableMouseButton;
}

Qt::MouseButton Settings::translatePoseRenderableMouseButton() const {
    return m_translatePoseRenderableMouseButton;
}

void Settings::setTranslatePoseRenderableMouseButton(
        const Qt::MouseButton &translatePoseRenderableMouseButton) {
    m_translatePoseRenderableMouseButton = translatePoseRenderableMouseButton;
}

Settings::Theme Settings::theme() const {
    return m_theme;
}

void Settings::setTheme(const Settings::Theme &theme) {
    m_theme = theme;
}

float Settings::click3DSize() const {
    return m_click3DSize;
}

void Settings::setClick3DSize(float click3DSize) {
    m_click3DSize = click3DSize;
}

QString Settings::pythonInterpreterPath() const {
    return m_pythonInterpreterPath;
}

void Settings::setPythonInterpreterPath(const QString &pythonInterpreterPath) {
    m_pythonInterpreterPath = pythonInterpreterPath;
}

QString Settings::loadSaveScriptPath() const {
    return m_loadSaveScriptPath;
}

void Settings::setLoadSaveScriptPath(const QString &loadSaveScriptPath) {
    m_loadSaveScriptPath = loadSaveScriptPath;
}

Settings::UsedLoadAndStoreStrategy Settings::usedLoadAndStoreStrategy() const {
    return m_usedLoadAndStoreStrategy;
}

void Settings::setUsedLoadAndStoreStrategy(UsedLoadAndStoreStrategy usedLoadAndStoreStrategy) {
    m_usedLoadAndStoreStrategy = usedLoadAndStoreStrategy;
}

int Settings::multisampleSamples() const {
    return m_multisampleSamples;
}

void Settings::setMultisampleSamples(int newMultisampleSamples) {
    m_multisampleSamples = newMultisampleSamples;
}

bool Settings::showFPSLabel() const {
    return m_showFPSLabel;
}

void Settings::setShowFPSLabel(bool newShowFPSLabel) {
    m_showFPSLabel = newShowFPSLabel;
}
