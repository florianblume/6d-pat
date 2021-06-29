#include "settingsstore.hpp"
#include "misc/global.hpp"

#include <QSettings>
#include <QDir>
#include <QDebug>

SettingsStore::SettingsStore(const QString &currentSettingsIdentifier) {
    setCurrentSettings(currentSettingsIdentifier);
}

QSharedPointer<Settings> SettingsStore::createEmptyPreferences(const QString &identifier) {
    return QSharedPointer<Settings>(new Settings(identifier));
}

void SettingsStore::saveCurrentSettings() {
    QSettings settings(ORGANIZATION, APPLICATION);
    const QString &identifier = PREFERENCES + m_currentSettings->identifier();
    settings.beginGroup(identifier);
    settings.setValue(IMAGES_PATH, m_currentSettings->imagesPath());
    settings.setValue(OBJECT_MODELS_PATH, m_currentSettings->objectModelsPath());
    settings.setValue(POSES_FILE_PATH, m_currentSettings->posesFilePath());
    settings.setValue(SEGMENTATION_IMAGES_PATH, m_currentSettings->segmentationImagesPath());
    settings.setValue(PYTHON_INTERPRETER_PATH, m_currentSettings->pythonInterpreterPath());
    settings.setValue(LOAD_SAVE_SCRIPT_PATH, m_currentSettings->loadSaveScriptPath());
    settings.setValue(USED_LOAD_AND_STORE_STRATEGY, m_currentSettings->usedLoadAndStoreStrategy());
    settings.setValue(ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[m_currentSettings->addCorrespondencePointMouseButton()]);
    settings.setValue(MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[m_currentSettings->moveBackgroundImageRenderableMouseButton()]);
    settings.setValue(SELECT_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[m_currentSettings->selectPoseRenderableMouseButton()]);
    settings.setValue(ROTATE_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[m_currentSettings->rotatePoseRenderableMouseButton()]);
    settings.setValue(TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[m_currentSettings->translatePoseRenderableMouseButton()]);
    settings.setValue(CLICK_3D_SIZE, m_currentSettings->click3DSize());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup(identifier + COLOR_CODES_GROUP);
    settings.remove("");
    for (auto objectModelIdentifier : currentSettings()->segmentationCodes().keys()) {
        settings.setValue(objectModelIdentifier, currentSettings()->segmentationCodeForObjectModel(objectModelIdentifier));
    }
    settings.endGroup();

    Q_EMIT currentSettingsChanged(m_currentSettings);
}

SettingsPtr SettingsStore::currentSettings() {
    return m_currentSettings;
}

void SettingsStore::setCurrentSettings(const QString &identifier) {
    SettingsPtr settingsPointer(new Settings(identifier));
    QSettings settings(ORGANIZATION, APPLICATION);
    const QString &fullIdentifier = PREFERENCES + identifier;
    settings.beginGroup(fullIdentifier);
    settingsPointer->setImagesPath(
                settings.value(IMAGES_PATH, Global::NO_PATH).toString());
    settingsPointer->setObjectModelsPath(
                settings.value(OBJECT_MODELS_PATH, Global::NO_PATH).toString());
    settingsPointer->setPosesFilePath(
                settings.value(POSES_FILE_PATH, Global::NO_PATH).toString());
    settingsPointer->setSegmentationImagePath(
                settings.value(SEGMENTATION_IMAGES_PATH, "").toString());
    settingsPointer->setPythonInterpreterPath(
                settings.value(PYTHON_INTERPRETER_PATH, Global::NO_PATH).toString());
    settingsPointer->setLoadSaveScriptPath(
                settings.value(LOAD_SAVE_SCRIPT_PATH, Global::NO_PATH).toString());
    settingsPointer->setUsedLoadAndStoreStrategy(
                settings.value(USED_LOAD_AND_STORE_STRATEGY, Settings::UsedLoadAndStoreStrategy::Default).toInt());
    settingsPointer->setAddCorrespondencePointMouseButton(
                Settings::MOUSE_BUTTONS.keys().at(
                    settings.value(ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON, Settings::MOUSE_BUTTONS[Qt::LeftButton]).toInt()));
    settingsPointer->setMoveBackgroundImageRenderableMouseButton(
                Settings::MOUSE_BUTTONS.keys().at(
                    settings.value(MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON, Settings::MOUSE_BUTTONS[Qt::MiddleButton]).toInt()));
    settingsPointer->setSelectPoseRenderableMouseButton(
                Settings::MOUSE_BUTTONS.keys().at(
                    settings.value(SELECT_POSE_RENDERABLE_MOUSE_BUTTON, Settings::MOUSE_BUTTONS[Qt::RightButton]).toInt()));
    settingsPointer->setTranslatePoseRenderableMouseButton(
                Settings::MOUSE_BUTTONS.keys().at(
                    settings.value(TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON, Settings::MOUSE_BUTTONS[Qt::LeftButton]).toInt()));
    settingsPointer->setRotatePoseRenderableMouseButton(
                Settings::MOUSE_BUTTONS.keys().at(
                    settings.value(ROTATE_POSE_RENDERABLE_MOUSE_BUTTON, Settings::MOUSE_BUTTONS[Qt::RightButton]).toInt()));
    settingsPointer->setClick3DSize(settings.value(CLICK_3D_SIZE, 0.5).toFloat());
    // TODO read mouse buttons
    settings.endGroup();

    settings.beginGroup(fullIdentifier + COLOR_CODES_GROUP);
    QStringList objectModelIdentifiers = settings.allKeys();
    for (const QString &objectModelIdentifier : objectModelIdentifiers) {
        const QString &colorCode = settings.value(objectModelIdentifier, "").toString();
        if (colorCode.compare("") != 0)
            settingsPointer->setSegmentationCodeForObjectModel(objectModelIdentifier, colorCode);
    }
    settings.endGroup();

    m_currentSettings = settingsPointer;
    Q_EMIT currentSettingsChanged(m_currentSettings);
}

const QString SettingsStore::ORGANIZATION = "Floretti Konfetti Inc.";
const QString SettingsStore::APPLICATION = "6D-PAT";
const QString SettingsStore::PREFERENCES = "preferences";
const QString SettingsStore::IMAGES_PATH = "imagesPath";
const QString SettingsStore::OBJECT_MODELS_PATH = "objectModelsPath";
const QString SettingsStore::POSES_FILE_PATH = "posesFilePath";
const QString SettingsStore::SEGMENTATION_IMAGES_PATH = "segmentationImagesPath";
const QString SettingsStore::COLOR_CODES = "colorCodes";
const QString SettingsStore::COLOR_CODES_GROUP = "-colorcodes";
const QString SettingsStore::PYTHON_INTERPRETER_PATH = "pythonInterpreterPath";
const QString SettingsStore::LOAD_SAVE_SCRIPT_PATH = "loadSaveScriptPath";
const QString SettingsStore::USED_LOAD_AND_STORE_STRATEGY = "usedLoadAndStoreStrategy";
const QString SettingsStore::ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON = "addCorrespondencePointMouseButton";
const QString SettingsStore::MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON = "moveBackgroundImageRenderableMouseButton";
const QString SettingsStore::SELECT_POSE_RENDERABLE_MOUSE_BUTTON = "selectPoseRenderableMouseButton";
const QString SettingsStore::ROTATE_POSE_RENDERABLE_MOUSE_BUTTON = "rotatePoseRenderableMouseButton";
const QString SettingsStore::TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON = "translatePoseRenderableMouseButton";
const QString SettingsStore::CLICK_3D_SIZE = "click3dsize";
