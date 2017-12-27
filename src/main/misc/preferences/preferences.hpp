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

    QString getIdentifier();

private:
    QMap<QString, QString> segmentationCodes;
    QString segmentationImageFilesSuffix;
    QString imageFilesExtension;
    QString imagesPath;
    QString objectModelsPath;
    QString correspondencesPath;

    QString identifier;
};

#endif // SETTINGSITEM_H
