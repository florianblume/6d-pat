#include "jsonloadandstorestrategy.hpp"
#include "misc/otiathelper.h"

#include <opencv2/core/mat.hpp>

#include <QDirIterator>
#include <QCollator>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QUuid>

const QStringList JsonLoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS = QStringList({"*.obj", "*.ply"});

static QString convertPathToSuffxFileName(const QString &pathToConvert, const QString &suffix, const QString &extension) {
    return QFileInfo(pathToConvert).completeBaseName() + suffix + extension;
}

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy() {
    // nothing to do here
}

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy(const QDir &imagesPath, const QDir &objectModelsPath,
                                                           const QDir &correspondencesFilePath)
    : imagesPath(imagesPath), objectModelsPath(objectModelsPath), correspondencesFilePath(correspondencesFilePath) {
}

JsonLoadAndStoreStrategy::~JsonLoadAndStoreStrategy() {
}

bool JsonLoadAndStoreStrategy::persistObjectImageCorrespondence(
        ObjectImageCorrespondence *objectImageCorrespondence, bool deleteCorrespondence) {
    // no need to check whether paths exist because setters do so already

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!pathExists(correspondencesFilePath)) {
        return false;
    }

}

QList<Image> JsonLoadAndStoreStrategy::loadImages() {
    QList<Image> images;

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!pathExists(imagesPath)) {
        return images;
    }

    QStringList fileFilter("*" + imageFilesExtension);
    QStringList files = imagesPath.entryList(fileFilter, QDir::Files, QDir::Name);

    for (int i = 0; i < files.size(); i += 2) {
        QString image = files[i];
        QString imageFilename = QFileInfo(image).fileName();
        //! Check the next image if it is the segmentation image of the currently inspected image
        int j = i + 1;
        float focalLengthX = 4781.91740099f;
        float focalLengthY = 4778.72123643f;
        float focalPointX = 973.66974847f;
        float focalPointY = 502.86220751f;

        // TODO: load parameters from json

        if (j < files.size()) {
            QString secondImage = files[j];
            QString secondImageFilename = QFileInfo(secondImage).fileName();

            //! This is the filename that the next image would have if it is a segmentation image
            //! If secondImage doesn't equal segmentationImageFilename then it is not a segmentation image,
            //! i.e. segmentation images are not provided in this folder or only for some images
            QString segmentationImageFilename = convertPathToSuffxFileName(image,
                                                                           segmentationImageFilesSuffix,
                                                                           imageFilesExtension);
            if (secondImageFilename.compare(segmentationImageFilename) == 0) {
                //! Apparently we found a segmentation image, i.e. add the second image as segmentation
                //! to the first
                images.push_back(Image(imageFilename, imagesPath.path(), segmentationImageFilename, focalLengthX, focalLengthY, focalPointX, focalPointY));
            } else {
                //! We didn't find a segmentation image, i.e. two separate images have to be added
                images.push_back(Image(imageFilename, imagesPath.path(), focalLengthX, focalLengthY, focalPointX, focalPointY));
                images.push_back(Image(secondImageFilename, imagesPath.path(), focalLengthX, focalLengthY, focalPointX, focalPointY));
            }
        } else {
            images.push_back(Image(imageFilename, imagesPath.path(), focalLengthX, focalLengthY, focalPointX, focalPointY));
        }
    }

    return images;
}

QList<ObjectModel> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModel> objectModels;

    //! See explanation under loadImages for why we don't throw an exception here
    if (!pathExists(objectModelsPath.path())) {
        return objectModels;
    }

    QDirIterator it(objectModelsPath.path(), OBJECT_MODEL_FILES_EXTENSIONS, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        //! We store only the filename as object model path, because that's
        //! the format of the ground truth file used by the neural network
        ObjectModel objectModel(fileInfo.fileName(), fileInfo.absolutePath());
        objectModels.append(objectModel);
    }

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(
        objectModels.begin(),
        objectModels.end(),
        [&collator](const ObjectModel &o1, const ObjectModel &o2)
        {
            return collator.compare(o1.getPath(), o2.getPath()) < 0;
        });

    return objectModels;
}

QMap<QString, const Image*> createImageMap(const QList<Image> &images) {
    QMap<QString, const Image*> imageMap;

    for (int i = 0; i < images.size(); i++) {
        imageMap[images.at(i).getImagePath()] = &(images.at(i));
    }

    return imageMap;
}

QMap<QString,const ObjectModel*> createObjectModelMap(const QList<ObjectModel> &objectModels) {
    QMap<QString, const ObjectModel*> objectModelMap;

    for (int i = 0; i < objectModels.size(); i++) {
        objectModelMap[objectModels.at(i).getPath()] = &(objectModels.at(i));
    }

    return objectModelMap;
}

QList<ObjectImageCorrespondence> JsonLoadAndStoreStrategy::loadCorrespondences(const QList<Image> &images, const QList<ObjectModel> &objectModels) {
    QList<ObjectImageCorrespondence> correspondences;

    //! See loadImages for why we don't throw an exception here
    if (!QFileInfo(correspondencesFilePath.path()).exists()) {
        return correspondences;
    }

    QFile jsonFile(correspondencesFilePath.path());
    if (jsonFile.open(QFile::ReadOnly)) {
        QMap<QString, const Image*> imageMap = createImageMap(images);
        QMap<QString, const ObjectModel*> objectModelMap = createObjectModelMap(objectModels);
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();
        foreach(const QString& imagePath, jsonObject.keys()) {
            QJsonArray entriesForImage = jsonObject[imagePath].toArray();
            foreach(const QJsonValue &correspondenceEntryRaw, entriesForImage) {
                QJsonObject correspondenceEntry = correspondenceEntryRaw.toObject();
                Q_ASSERT(correspondenceEntry.contains("R"));
                Q_ASSERT(correspondenceEntry.contains("t"));
                Q_ASSERT(correspondenceEntry.contains("obj"));

                //! Read rotation vector from json file
                QJsonArray rotationRows = correspondenceEntry["R"].toArray();
                QJsonArray::iterator it = rotationRows.begin();
                QJsonArray firstRow = it[0].toArray();
                QJsonArray secondRow = it[1].toArray();
                QJsonArray thirdRow = it[2].toArray();
                cv::Mat rotationMatrix = (cv::Mat_<float>(3,3) <<
                   (float) firstRow[0].toDouble(),  (float) firstRow[1].toDouble(),  (float) firstRow[2].toDouble(),
                   (float) secondRow[0].toDouble(), (float) secondRow[1].toDouble(), (float) secondRow[2].toDouble(),
                   (float) thirdRow[0].toDouble(),  (float) thirdRow[1].toDouble(),  (float) thirdRow[2].toDouble());
                cv::Vec3f rotationVector = OtiatHelper::rotationMatrixToEulerAngles(rotationMatrix);
                //! Convert to Qt vector format
                QVector3D qtRotationVector = QVector3D(rotationVector[0],
                                                       rotationVector[1],
                                                       rotationVector[2]);
                qtRotationVector *= 180.f / M_PI;

                //! Read position vector from json file
                cv::Vec3f translationVector;
                QJsonArray translation = correspondenceEntry["t"].toArray();
                int index = 0;
                foreach(const QJsonValue &translationEntry, translation) {
                    translationVector[index] = (float) translationEntry.toDouble();
                    index++;
                }
                //! Convert to Qt vector format
                QVector3D qtTranslationVector = QVector3D(translationVector[0],
                                                          translationVector[1],
                                                          translationVector[2]);
                QString objectModelPath = correspondenceEntry["obj"].toString();
                const Image *image = imageMap.value(imagePath);
                const ObjectModel *objectModel = objectModelMap.value(objectModelPath);
                if (image && objectModel)
                    //! If either is NULL, we do not manage the image or object model
                    //! specified in the JSON file, that's why we just skip the entry
                    correspondences.append(ObjectImageCorrespondence(QUuid::createUuid().toString(),
                                                                 qtTranslationVector,
                                                                 qtRotationVector,
                                                                 image,
                                                                 objectModel));
            }
        }
    }

    return correspondences;
}

bool JsonLoadAndStoreStrategy::setImagesPath(const QDir &path) {
    if (!pathExists(path))
        return false;
    if (imagesPath == path)
        return true;

    imagesPath = path;

    emit imagesChanged();

    return true;
}

QDir JsonLoadAndStoreStrategy::getImagesPath() const {
    return  imagesPath.path();
}

bool JsonLoadAndStoreStrategy::setObjectModelsPath(const QDir &path) {
    if (!pathExists(path))
        return false;
    if (objectModelsPath == path)
        return true;

    objectModelsPath = path;

    emit objectModelsChanged();

    return true;
}

QDir JsonLoadAndStoreStrategy::getObjectModelsPath() const {
    return objectModelsPath;
}

bool JsonLoadAndStoreStrategy::setCorrespondencesFilePath(const QDir &path) {
    if (!QFileInfo(path.path()).exists())
        return false;
    if (correspondencesFilePath == path)
        return true;

    correspondencesFilePath = path;

    emit correspondencesChanged();

    return true;
}

QDir JsonLoadAndStoreStrategy::getCorrespondencesFilePath() const {
    return correspondencesFilePath.path();
}

void JsonLoadAndStoreStrategy::setSegmentationImageFilesSuffix(const QString &suffix) {
    //! Only set suffix if it differs from the suffix before because we then have to reload images
    if (suffix.compare("") != 0 && segmentationImageFilesSuffix.compare("") != 0) {
        segmentationImageFilesSuffix = suffix;
        emit imagesChanged();
    }
}

QString JsonLoadAndStoreStrategy::getSegmentationImageFilesSuffix() {
    return segmentationImageFilesSuffix;
}

void JsonLoadAndStoreStrategy::setImageFilesExtension(const QString &extension) {
    if (extension.compare("") && imageFilesExtension.compare(extension) != 0) {
        imageFilesExtension = extension;
        emit imagesChanged();
    }
}

QString JsonLoadAndStoreStrategy::getImageFilesExtension() {
    return imageFilesExtension;
}
