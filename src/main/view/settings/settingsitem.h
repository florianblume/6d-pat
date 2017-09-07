#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include "model/objectmodel.hpp"
#include "model/modelmanager.hpp"
#include <QString>
#include <QSettings>
#include <map>

class SettingsItem
{
public:
    SettingsItem(QString identifier, ModelManager* modelManager);
    ~SettingsItem();

    void setSegmentationCodeForObjectModel(const QString &code, ObjectModel* forModel);

    map<QString, ObjectModel *> getSegmentationCodes() const;
    void setSegmentationCodes(const map<QString, ObjectModel *> &value);

    QString getSegmentationCodeForObjectModel(ObjectModel* objectModel);

    QString getSegmentationImageFilesSuffix() const;
    void setSegmentationImageFilesSuffix(const QString &value);

    QString getImageFilesExtension() const;
    void setImageFilesExtension(const QString &value);

    QString getImagesPath() const;
    void setImagesPath(const QString &value);

    QString getObjectModelsPath() const;
    void setObjectModelsPath(const QString &value);

    QString getCorrespondencesPath() const;
    void setCorrespondencesPath(const QString &value);

private:
    ModelManager* modelManager;
    map<QString, ObjectModel*> segmentationCodes;
    map<ObjectModel*, QString> inverseSegmentationCodes;
    QString segmentationImageFilesSuffix;
    QString imageFilesExtension;
    QString imagesPath;
    QString objectModelsPath;
    QString correspondencesPath;

    QString identifier;
};

#endif // SETTINGSITEM_H
