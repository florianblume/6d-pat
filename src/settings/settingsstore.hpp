#ifndef PREFERENCESSTORE_H
#define PREFERENCESSTORE_H

#include "settings.hpp"

#include <QSharedPointer>
#include <QString>
#include <QObject>

/*!
 * \brief The SettingsStore class is responsible for loading and saving settings objects.
 * It has a currentSettings which can be retrieved and saved. Setting the current settings identifier
 * makes the SettingsStore update its currentSettings and emit a signal that it has changed.
 */
class SettingsStore : public QObject {

Q_OBJECT

public:
    /*!
     * \brief SettingsStore creates the SettingsStore and loads the settings using the given identifier
     * \param currentSettingsIdentifier the identifier to load the initial settings with
     */
    SettingsStore(const QString &currentSettingsIdentifier);

    /*!
     * \brief saveCurrentSettings saves, i.e. persits, the current settings. By obtaining the pointer
     * to the current settings using the currentSettings() method and modifying its values it's
     * possible to alter the current settings. After having made changes to the current settings
     * the user needs to call saveCurrentSettings() to persist the settings and make them available
     * throughout the program.
     */
    void saveCurrentSettings();

    /*!
     * \brief setCurrentSettings sets the identifier of the current settings of this SettingsStore.
     * This triggers loading the settings using the settings identifier and emitting a signal that
     * the current settings have changed.
     * \param currentSettingsIdentifier the identifier to load the new current settings with
     */
    void setCurrentSettings(const QString &currentSettingsIdentifier);

    /*!
     * \brief currentSettings can be used to obtain the current settings of this SettingsStore
     * \return the current settings
     */
    Settings currentSettings();

Q_SIGNALS:
    /*!
     * \brief currentSettingsChanged is emitted whenever the current settings of this SettingsStore
     * are saved or the identifier of the current settings is changed, causing the SettingsStore
     * to reload the current settings using the provided identifier
     * \param currentSettings the current settings of this SettingsStore with all updated parameters
     */
    void currentSettingsChanged(const Settings &currentSettings);

private:
    SettingsPtr createEmptyPreferences(const QString &identifier);

private:
    SettingsPtr m_currentSettings;

    static const QString ORGANIZATION;
    static const QString APPLICATION;
    static const QString PREFERENCES;
    static const QString IMAGES_PATH;
    static const QString OBJECT_MODELS_PATH;
    static const QString POSES_FILE_PATH;
    static const QString SEGMENTATION_IMAGES_PATH;
    static const QString COLOR_CODES;
    static const QString COLOR_CODES_GROUP;
    static const QString PYTHON_INTERPRETER_PATH;
    static const QString LOAD_SAVE_SCRIPT_PATH;
    static const QString USED_LOAD_AND_STORE_STRATEGY;
    static const QString ADD_CORRESPONDENCE_POINT_MOUSE_BUTTON;
    static const QString MOVE_BACKGROUNDIMAGE_RENDERABLE_MOUSE_BUTTON;
    static const QString SELECT_POSE_RENDERABLE_MOUSE_BUTTON;
    static const QString ROTATE_POSE_RENDERABLE_MOUSE_BUTTON;
    static const QString TRANSLATE_POSE_RENDERABLE_MOUSE_BUTTON;
    static const QString CLICK_3D_SIZE;
    static const QString MULTISAMPLING_SAMLPES;
    static const QString SHOW_FPS_LABEL;
};

typedef QSharedPointer<SettingsStore> SettingsStorePtr;

#endif // PREFERENCESSTORE_H
