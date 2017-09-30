#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include "model/objectmodel.hpp"
#include "model/modelmanager.hpp"
#include <QString>
#include <QMap>

class SettingsItem
{
public:
    SettingsItem(QString identifier, ModelManager* modelManager);
    ~SettingsItem();

    void setSegmentationCodeForObjectModel(const ObjectModel* objectModel, const QString &code);
    void removeSegmentationCodeForObjectModel(const ObjectModel* objectModel);

    void getSegmentationCodes(QMap<const ObjectModel*, QString> &codes) const;
    void setSegmentationCodes(const QMap<const ObjectModel*, QString> codes);

    QString getSegmentationCodeForObjectModel(const ObjectModel* objectModel);

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
    QMap<const ObjectModel*, QString> segmentationCodes;
    QString segmentationImageFilesSuffix;
    QString imageFilesExtension;
    QString imagesPath;
    QString objectModelsPath;
    QString correspondencesPath;

    QString identifier;
};

#endif // SETTINGSITEM_H
