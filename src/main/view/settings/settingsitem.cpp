#include "settingsitem.h"


SettingsItem::SettingsItem(QString identifier, ModelManager* modelManager) : identifier(identifier) {
    this->modelManager = modelManager;
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("settingsitem-" + identifier);

    /*
    imagesPath = settings.value("imagesPath", "").toString();
    objectModelsPath = settings.value("objectModelsPath", "").toString();
    correspondencesPath = settings.value("correspondencesPath", "").toString();
    segmentationImageFilesSuffix = settings.value("segmentationImageFilesSuffix", "").toString();
    imageFilesExtension = settings.value("imageFilesExtension", "").toString();
    */

    QList<ObjectModel*> objectModels;
    modelManager->getObjectModels(objectModels);

    for (uint i = 0; i < objectModels.size(); i++) {
        ObjectModel* model = objectModels.at(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        QString storedCode = settings.value(objectModelIdentifier, "").toString();
        segmentationCodes[storedCode] = model;
        inverseSegmentationCodes[model] = storedCode;
    }

    settings.endGroup();
}

SettingsItem::~SettingsItem() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("settingsitem-" + identifier);

    /*
    settings.setValue("imagesPath", imagesPath);
    settings.setValue("objectModelsPath", objectModelsPath);
    settings.setValue("correspondencesPath", correspondencesPath);
    settings.setValue("segmentationImageFilesSuffix", segmentationImageFilesSuffix);
    settings.setValue("imageFilesExtension", imageFilesExtension);
    */

    QList<ObjectModel*> objectModels;
    modelManager->getObjectModels(objectModels);

    for (uint i = 0; i < objectModels.size(); i++) {
        ObjectModel* model = objectModels.at(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        settings.setValue(model->getAbsolutePath(), getSegmentationCodeForObjectModel(model));
    }

    settings.endGroup();
}

QString SettingsItem::getSegmentationImageFilesSuffix() const
{
    return segmentationImageFilesSuffix;
}

void SettingsItem::setSegmentationImageFilesSuffix(const QString &value)
{
    segmentationImageFilesSuffix = value;
}

QString SettingsItem::getImageFilesExtension() const
{
    return imageFilesExtension;
}

void SettingsItem::setImageFilesExtension(const QString &value)
{
    imageFilesExtension = value;
}

QString SettingsItem::getImagesPath() const
{
    return imagesPath;
}

void SettingsItem::setImagesPath(const QString &value)
{
    imagesPath = value;
}

QString SettingsItem::getObjectModelsPath() const
{
    return objectModelsPath;
}

void SettingsItem::setObjectModelsPath(const QString &value)
{
    objectModelsPath = value;
}

QString SettingsItem::getCorrespondencesPath() const
{
    return correspondencesPath;
}

void SettingsItem::setCorrespondencesPath(const QString &value)
{
    correspondencesPath = value;
}

map<QString, ObjectModel *> SettingsItem::getSegmentationCodes() const
{
    return segmentationCodes;
}

void SettingsItem::setSegmentationCodes(const map<QString, ObjectModel *> &value)
{
    segmentationCodes = value;
}

void SettingsItem::setSegmentationCodeForObjectModel(const QString &code, ObjectModel *forModel) {
    segmentationCodes[code] = forModel;
    inverseSegmentationCodes[forModel] = code;
}

QString SettingsItem::getSegmentationCodeForObjectModel(ObjectModel* objectModel) {
    return inverseSegmentationCodes[objectModel];
}
