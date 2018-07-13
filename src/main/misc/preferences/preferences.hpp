#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include "model/objectmodel.hpp"
#include <QString>
#include <QMap>

class Preferences
{
public:
    Preferences(QString identifier);
    Preferences(const Preferences &preferences);
    ~Preferences();

    void setSegmentationCodeForObjectModel(const QString &identifier, const QString &code);
    void removeSegmentationCodeForObjectModel(const QString &identifier);

    QMap<QString, QString> getSegmentationCodes() const;
    void setSegmentationCodes(const QMap<QString, QString> codes);

    QString getSegmentationCodeForObjectModel(const QString &identifier);

    QString getSegmentationImagesPath() const;
    void setSegmentationImagePath(const QString &value);

    QString getImagesPath() const;
    void setImagesPath(const QString &value);

    QString getObjectModelsPath() const;
    void setObjectModelsPath(const QString &value);

    QString getCorrespondencesFilePath() const;
    void setCorrespondencesFilePath(const QString &value);

    QString getIdentifier();

    QString getTrainingScriptPath() const;
    void setTrainingScriptPath(const QString &value);

    QString getInferenceScriptPath() const;
    void setInferenceScriptPath(const QString &value);

    QString getNetworkConfigPath() const;
    void setNetworkConfigPath(const QString &value);

    QString getPythonInterpreterPath() const;
    void setPythonInterpreterPath(const QString &value);

private:
    QMap<QString, QString> segmentationCodes;
    QString segmentationImagesPath;
    QString imagesPath;
    QString objectModelsPath;
    QString correspondencesFilePath;
    QString pythonInterpreterPath;
    QString trainingScriptPath;
    QString inferenceScriptPath;
    QString networkConfigPath;

    QString identifier;
};

#endif // SETTINGSITEM_H
