#ifndef PREFERENCESSTORE_H
#define PREFERENCESSTORE_H

#include "settings.hpp"

#include <QSharedPointer>
#include <QString>
#include <QObject>

class SettingsStore : public QObject
{
    Q_OBJECT

public:
    SettingsStore();
    SettingsPtr createEmptyPreferences(const QString &identifier);
    void savePreferences(const Settings &settings);
    SettingsPtr loadPreferencesByIdentifier(const QString &identifier);

signals:
    void settingsChanged(SettingsPtr settingsToSave);

private:
    static const QString PREFERENCES;
    static const QString IMAGES_PATH;
    static const QString OBJECT_MODELS_PATH;
    static const QString POSES_FILE_PATH;
    static const QString SEGMENTATION_IMAGES_PATH;
    static const QString COLOR_CODES;
    static const QString PYTHON_INTERPRETER_PATH;
    static const QString TRAINING_SCRIPT_PATH;
    static const QString INFERENCE_SCRIPT_PATH;
    static const QString NETWORK_CONFIG_PATH;
    static const QString ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON;
    static const QString MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON;
    static const QString SELECT_POSE_RENDERABLE_MOUSE_BUTTON;
    static const QString ROTATE_POSE_RENDERABLE_MOUSE_BUTTON;
    static const QString TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON;
};

#endif // PREFERENCESSTORE_H
