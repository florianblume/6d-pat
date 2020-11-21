#include "settingsstore.hpp"
#include <QSettings>
#include <QDir>
#include <QDebug>

SettingsStore::SettingsStore() {
    m_currentSettings = loadPreferencesByIdentifier("default");
}

QSharedPointer<Settings> SettingsStore::createEmptyPreferences(const QString &identifier) {
    return QSharedPointer<Settings>(new Settings(identifier));
}

void SettingsStore::savePreferences(const Settings &settingsToSave) {
    QSettings settings("FlorettiKonfetti Inc.", "6D-PAT");
    const QString &identifier = PREFERENCES + settingsToSave.identifier();
    settings.beginGroup(identifier);
    settings.setValue(IMAGES_PATH, settingsToSave.imagesPath());
    settings.setValue(OBJECT_MODELS_PATH, settingsToSave.objectModelsPath());
    settings.setValue(POSES_FILE_PATH, settingsToSave.posesFilePath());
    settings.setValue(SEGMENTATION_IMAGES_PATH, settingsToSave.segmentationImagesPath());
    settings.setValue(PYTHON_INTERPRETER_PATH, settingsToSave.pythonInterpreterPath());
    settings.setValue(TRAINING_SCRIPT_PATH, settingsToSave.trainingScriptPath());
    settings.setValue(INFERENCE_SCRIPT_PATH, settingsToSave.inferenceScriptPath());
    settings.setValue(NETWORK_CONFIG_PATH, settingsToSave.networkConfigPath());
    settings.setValue(ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[settingsToSave.addCorrespondencePointMouseButton()]);
    settings.setValue(MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[settingsToSave.moveBackgroundImageRenderableMouseButton()]);
    settings.setValue(SELECT_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[settingsToSave.selectPoseRenderableMouseButton()]);
    settings.setValue(ROTATE_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[settingsToSave.rotatePoseRenderableMouseButton()]);
    settings.setValue(TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON,
                      Settings::MOUSE_BUTTONS[settingsToSave.translatePoseRenderableMouseButton()]);
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup(identifier + "-colorcodes");
    settings.remove("");
    for (auto objectModelIdentifier : settingsToSave.segmentationCodes().keys()) {
        settings.setValue(objectModelIdentifier, settingsToSave.segmentationCodeForObjectModel(objectModelIdentifier));
    }
    settings.endGroup();

    m_currentSettings.reset(new Settings(settingsToSave));

    emit settingsChanged(SettingsPtr(new Settings(settingsToSave)));
}

SettingsPtr SettingsStore::currentSettings() {
    return m_currentSettings;
}

QSharedPointer<Settings> SettingsStore::loadPreferencesByIdentifier(const QString &identifier) {
    SettingsPtr settingsPointer(new Settings(identifier));
    QSettings settings("FlorettiKonfetti Inc.", "6D-PAT");
    const QString &fullIdentifier = PREFERENCES + identifier;
    settings.beginGroup(fullIdentifier);
    settingsPointer->setImagesPath(
                settings.value(IMAGES_PATH, QDir::homePath()).toString());
    settingsPointer->setObjectModelsPath(
                settings.value(OBJECT_MODELS_PATH, QDir::homePath()).toString());
    settingsPointer->setPosesFilePath(
                settings.value(POSES_FILE_PATH, QDir::homePath()).toString());
    settingsPointer->setSegmentationImagePath(
                settings.value(SEGMENTATION_IMAGES_PATH, "").toString());
    settingsPointer->setPythonInterpreterPath(
                settings.value(PYTHON_INTERPRETER_PATH, "").toString());
    settingsPointer->setTrainingScriptPath(
                settings.value(TRAINING_SCRIPT_PATH, "").toString());
    settingsPointer->setInferenceScriptPath(
                settings.value(INFERENCE_SCRIPT_PATH, "").toString());
    settingsPointer->setNetworkConfigPath(
                settings.value(NETWORK_CONFIG_PATH, "").toString());
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
    // TODO read mouse buttons
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


const QString SettingsStore::PREFERENCES = "preferences";
const QString SettingsStore::IMAGES_PATH = "imagesPath";
const QString SettingsStore::OBJECT_MODELS_PATH = "objectModelsPath";
const QString SettingsStore::POSES_FILE_PATH = "posesFilePath";
const QString SettingsStore::SEGMENTATION_IMAGES_PATH = "segmentationImagesPath";
const QString SettingsStore::COLOR_CODES = "colorCodes";
const QString SettingsStore::PYTHON_INTERPRETER_PATH = "pythonInterpreterPath";
const QString SettingsStore::TRAINING_SCRIPT_PATH = "trainingScriptPath";
const QString SettingsStore::INFERENCE_SCRIPT_PATH = "inferenceScriptPath";
const QString SettingsStore::NETWORK_CONFIG_PATH = "networkConfigPath";
const QString SettingsStore::ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON = "addCorrespondencePointMouseButton";
const QString SettingsStore::MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON = "moveBackgroundImageRenderableMouseButton";
const QString SettingsStore::SELECT_POSE_RENDERABLE_MOUSE_BUTTON = "selectPoseRenderableMouseButton";
const QString SettingsStore::ROTATE_POSE_RENDERABLE_MOUSE_BUTTON = "rotatePoseRenderableMouseButton";
const QString SettingsStore::TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON = "translatePoseRenderableMouseButton";
