#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include "model/objectmodel.hpp"
#include <QString>
#include <QMap>
#include <QSharedPointer>

class Settings {

public:
    static const QMap<Qt::MouseButton, int> MOUSE_BUTTONS;

    enum Theme {
      Light,
      Dark
    };

    enum UsedLoadAndStoreStrategy {
        Default,
        Python
    };

    Settings(const QString &identifier);
    Settings(const Settings &settings);
    ~Settings();

    void setSegmentationCodeForObjectModel(const QString &identifier, const QString &code);
    void removeSegmentationCodeForObjectModel(const QString &identifier);

    QMap<QString, QString> segmentationCodes() const;
    void setSegmentationCodes(const QMap<QString, QString> codes);

    QString segmentationCodeForObjectModel(const QString &identifier) const;

    QString segmentationImagesPath() const;
    void setSegmentationImagePath(const QString &value);

    QString imagesPath() const;
    void setImagesPath(const QString &value);

    QString objectModelsPath() const;
    void setObjectModelsPath(const QString &value);

    QString posesFilePath() const;
    void setPosesFilePath(const QString &value);

    QString identifier() const;

    Qt::MouseButton addCorrespondencePointMouseButton() const;
    void setAddCorrespondencePointMouseButton(
            const Qt::MouseButton &addCorrespondencePointMouseButton);

    Qt::MouseButton moveBackgroundImageRenderableMouseButton() const;
    void setMoveBackgroundImageRenderableMouseButton(
            const Qt::MouseButton &moveBackgroundImageRenderableMouseButton);

    Qt::MouseButton selectPoseRenderableMouseButton() const;
    void setSelectPoseRenderableMouseButton(
            const Qt::MouseButton &selectPoseRenderableMouseButton);

    Qt::MouseButton rotatePoseRenderableMouseButton() const;
    void setRotatePoseRenderableMouseButton(
            const Qt::MouseButton &rotatePoseRenderableMouseButton);

    Qt::MouseButton translatePoseRenderableMouseButton() const;
    void setTranslatePoseRenderableMouseButton(
            const Qt::MouseButton &translatePoseRenderableMouseButton);

    Theme theme() const;
    void setTheme(const Theme &theme);

    float click3DSize() const;
    void setClick3DSize(float click3DSize);

    QString pythonInterpreterPath() const;
    void setPythonInterpreterPath(const QString &pythonInterpreterPath);

    QString loadSaveScriptPath() const;
    void setLoadSaveScriptPath(const QString &loadSaveScriptPath);

    UsedLoadAndStoreStrategy usedLoadAndStoreStrategy() const;
    void setUsedLoadAndStoreStrategy(UsedLoadAndStoreStrategy usedLoadAndStoreStrategy);

    int multisampleSamples() const;
    void setMultisampleSamples(int newMultisampleSamples);

    bool showFPSLabel() const;
    void setShowFPSLabel(bool newShowFPSLabel);

private:
    QString m_identifier;

    QMap<QString, QString> m_segmentationCodes;
    QString m_segmentationImagesPath;
    QString m_imagesPath;
    QString m_objectModelsPath;
    QString m_posesFilePath;
    QString m_pythonInterpreterPath;
    QString m_loadSaveScriptPath;
    UsedLoadAndStoreStrategy m_usedLoadAndStoreStrategy;
    Qt::MouseButton m_addCorrespondencePointMouseButton;
    Qt::MouseButton m_moveBackgroundImageRenderableMouseButton;
    Qt::MouseButton m_selectPoseRenderableMouseButton;
    Qt::MouseButton m_rotatePoseRenderableMouseButton;
    Qt::MouseButton m_translatePoseRenderableMouseButton;
    float m_click3DSize;
    Theme m_theme;
    int m_multisampleSamples = 2;
    bool m_showFPSLabel = true;
};

typedef QSharedPointer<Settings> SettingsPtr;

#endif // SETTINGSITEM_H
