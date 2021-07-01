#include "jsonloadandstorestrategy.hpp"
#include "misc/generalhelper.hpp"
#include "misc/global.hpp"

#include <opencv2/core/mat.hpp>

#include <QSharedPointer>
#include <QDirIterator>
#include <QCollator>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QDir>
#include <QThread>

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy()  {
}

JsonLoadAndStoreStrategy::~JsonLoadAndStoreStrategy() {
}

bool JsonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {

    // Read in the camera parameters from the JSON file
    QFileInfo info(m_posesFilePath);
    QFile jsonFile(m_posesFilePath);

    if (!info.isFile()) {
        Q_EMIT error(tr("Failed to persist pose. Poses file is not a file."));
        return false;
    }

    if (!jsonFile.open(QFile::ReadWrite)) {
        Q_EMIT error(tr("Failed to persist pose. Poses file could not be read."));
        return false;
    }

    QByteArray data = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
    if (jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();

        QString imagePath = objectImagePose.image()->imagePath();
        QJsonArray entriesForImage;

        if (deletePose) {
            if (jsonObject.contains(imagePath)) {
                entriesForImage = jsonObject[imagePath].toArray();
                int index = 0;
                for(const QJsonValue &entry : entriesForImage) {
                    QJsonObject entryObject = entry.toObject();
                    if (entryObject["id"] == objectImagePose.id()) {
                        entriesForImage.removeAt(index);
                    }
                    index++;
                }
                jsonObject[imagePath] = entriesForImage;
            }
        } else {
            //! Preparation of 3D data for the JSON file
            QMatrix3x3 rotationMatrix = objectImagePose.rotation().toRotationMatrix();
            QJsonArray rotationMatrixArray;
            rotationMatrixArray << rotationMatrix(0, 0) << rotationMatrix(0, 1) << rotationMatrix(0, 2)
                                << rotationMatrix(1, 0) << rotationMatrix(1, 1) << rotationMatrix(1, 2)
                                << rotationMatrix(2, 0) << rotationMatrix(2, 1) << rotationMatrix(2, 2);
            QVector3D positionVector = objectImagePose.position();
            QJsonArray positionVectorArray;
            positionVectorArray << positionVector[0] << positionVector[1] << positionVector[2];
            //! Check if any entries for the image exist
            if (jsonObject.contains(imagePath)) {
                //! There are some entries already and we check whether the pose
                //! already exists
                entriesForImage = jsonObject[imagePath].toArray();
                //! We have to check whether our pose exists, and if it does, only update it
                //! If we don't find it we have to create it anew and add it to the list of poses
                bool entryFound = false;
                //! Keep track of the index if we find an existing pose
                int index = 0;
                //! Create new entry object, as we can't modify the exisiting ones directly somehow
                QJsonObject entry;
                entry["id"] = objectImagePose.id();
                entry["obj"] = objectImagePose.objectModel()->path();
                entry["R"] = rotationMatrixArray;
                entry["t"] = positionVectorArray;

                for(const QJsonValue &_entry : entriesForImage) {
                    QJsonObject entryObject = _entry.toObject();
                    if (entryObject["id"] == objectImagePose.id()) {
                        entryFound = true;
                        entriesForImage[index] = entry;
                    }
                    index++;
                }

                if (!entryFound) {
                    entriesForImage << entry;
                }
            } else {
                QJsonObject newEntry;
                newEntry["id"] = objectImagePose.id();
                newEntry["obj"] = objectImagePose.objectModel()->path();
                newEntry["t"] = positionVectorArray;
                newEntry["R"] = rotationMatrixArray;
                entriesForImage << newEntry;
            }
        }
        m_ignorePosesFileChanged = true;
        jsonObject[imagePath] = entriesForImage;
        jsonFile.resize(0);
        jsonFile.write(QJsonDocument(jsonObject).toJson());
    } else {
        // TODO JsonDocument is null, ie an error occured
    }

    return true;
}

static QMatrix3x3 rotVectorFromJsonRotMatrix(QJsonArray &jsonRotationMatrix) {
    float values[9] = {
        (float) jsonRotationMatrix[0].toDouble(),
        (float) jsonRotationMatrix[1].toDouble(),
        (float) jsonRotationMatrix[2].toDouble(),
        (float) jsonRotationMatrix[3].toDouble(),
        (float) jsonRotationMatrix[4].toDouble(),
        (float) jsonRotationMatrix[5].toDouble(),
        (float) jsonRotationMatrix[6].toDouble(),
        (float) jsonRotationMatrix[7].toDouble(),
        (float) jsonRotationMatrix[8].toDouble()};
    QMatrix3x3 rotationMatrix = QMatrix3x3(values);
    return rotationMatrix;
}

static ImagePtr createImageWithJsonParams(const QString& filename, const QString &segmentationFilename,
                                          const QString &imagesPath, QJsonObject &json) {
    QJsonObject parameters = json[filename].toObject();
    if (!parameters.contains("K")) {
        return ImagePtr();
    }
    QJsonArray cameraMatrix = parameters["K"].toArray();
    float values[9] = {
        (float) cameraMatrix[0].toDouble(),
        (float) cameraMatrix[1].toDouble(),
        (float) cameraMatrix[2].toDouble(),
        (float) cameraMatrix[3].toDouble(),
        (float) cameraMatrix[4].toDouble(),
        (float) cameraMatrix[5].toDouble(),
        (float) cameraMatrix[6].toDouble(),
        (float) cameraMatrix[7].toDouble(),
        (float) cameraMatrix[8].toDouble()};
    QMatrix3x3 qtCameraMatrix = QMatrix3x3(values);
    float nearPlane = 50;
    float farPlane = 2000;
    if (parameters.contains("nearPlane")) {
        nearPlane = (float) parameters["nearPlane"].toDouble();
    }
    if (parameters.contains("farPlane")) {
        farPlane = (float) parameters["farPlane"].toDouble();
    }
    return ImagePtr(new Image(filename, segmentationFilename, imagesPath, qtCameraMatrix,
                              nearPlane, farPlane));
}

QList<ImagePtr> JsonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;
    m_imagesWithInvalidData.clear();

    if (m_imagesPath == Global::NO_PATH) {
        // The only time when the images path can be equal to the NO_PATH is
        // when the program is first started -> then we show a hint in the
        // breaadcrumbs to select an actual path, i.e. not a problem that we
        // return early here
        return images;
    }

    // we do not need to throw an exception here, the only time the path cannot exist
    // is if this strategy was constructed with an empty path, all other methods of
    // setting the path check if the path exists
    if (!QFileInfo(m_imagesPath).exists()) {
        Q_EMIT error(tr("Failed to load images. Images path does not exist."));
        return images;
    } else if (!QFileInfo(m_imagesPath).isDir()) {
        Q_EMIT error(tr("Failed to load images. Images path is not a folder."));
        return images;
    } else if (m_segmentationImagesPath != "" && !QFileInfo(m_segmentationImagesPath).exists()) {
        Q_EMIT error(tr("Failed to load segmentation images. Segmentation images path does not exist."));
        return images;
    }

    QStringList imageFiles = QDir(m_imagesPath).entryList(IMAGE_FILES_EXTENSIONS, QDir::Files, QDir::Name);
    QStringList segmentationImageFiles =
            QDir(m_segmentationImagesPath).entryList(IMAGE_FILES_EXTENSIONS, QDir::Files, QDir::Name);
    // Also ensure that the number of elements is the same
    bool segmentationImagesPathSet = m_segmentationImagesPath != ""
            && imageFiles.size() == segmentationImageFiles.size();

    // Sort both list the have the corresponding image and segmentation image at the
    // same position
    QCollator collator;
    collator.setNumericMode(true);

    std::sort(
        imageFiles.begin(),
        imageFiles.end(),
        [&collator](const QString &s1, const QString &s2)
        {
            return collator.compare(s1, s2) < 0;
        });
    std::sort(
        segmentationImageFiles.begin(),
        segmentationImageFiles.end(),
        [&collator](const QString &s1, const QString &s2)
        {
            return collator.compare(s1, s2) < 0;
        });

    // Read in the camera parameters from the JSON file
    QFile jsonFile(QDir(m_imagesPath).filePath("info.json"));

    bool foundImageWithInvalidCameraMatrix = false;

    if (imageFiles.size() > 0 && jsonFile.open(QFile::ReadOnly)) {
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        if (jsonDocument.isNull()) {
            QJsonObject jsonObject = jsonDocument.object();
            for (int i = 0; i < imageFiles.size(); i ++) {
                QString image = imageFiles[i];
                QString imageFilename = QFileInfo(image).fileName();
                ImagePtr newImage;
                if (segmentationImagesPathSet) {
                    if (i > segmentationImageFiles.size() - 1) {
                        Q_EMIT error(tr("Failed to load images. Number of segmentation images "
                                        "does not match number of images."));
                        return images;
                    }
                    QString segmentationImageFile = segmentationImageFiles[i];
                    QString segmentationImageFilePath =
                            QDir(m_segmentationImagesPath).absoluteFilePath(segmentationImageFile);
                    newImage = createImageWithJsonParams(imageFilename,
                                                         segmentationImageFilePath,
                                                         m_imagesPath,
                                                         jsonObject);
                } else {
                    newImage = createImageWithJsonParams(imageFilename,
                                                         "",
                                                         m_imagesPath,
                                                         jsonObject);
                }
                if (!newImage) {
                    // This can only happen when the camera matrix is invalid
                    foundImageWithInvalidCameraMatrix = true;
                    m_imagesWithInvalidData.append(imageFilename);
                } else {
                    images.push_back(newImage);
                }
            }
        } else {
            //  TODO Document is null, ie there was an error
        }
    } else if (imageFiles.size() > 0) {
        // Only if we can read images but do not find the JSON info file we raise the exception
        Q_EMIT error(tr("Failed to load images. Camera info file info.json does not exist."));
    } else if (imageFiles.size() == 0) {
        Q_EMIT error(tr("No images found at give location."));
    }

    if (foundImageWithInvalidCameraMatrix) {
        Q_EMIT error(tr("There were images with invalid camera matrices."));
    }

    return images;
}

QList<ObjectModelPtr> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModelPtr> objectModels;

    if (m_objectModelsPath == Global::NO_PATH) {
        // The only time when the object models path can be equal to the NO_PATH is
        // when the program is first started -> then we show a hint in the
        // breaadcrumbs to select an actual path, i.e. not a problem that we
        // return early here
        return objectModels;
    }

    // See explanation under loadImages for why we don't throw an exception here
    QFileInfo info(m_objectModelsPath);
    if (!info.exists()) {
        Q_EMIT error(tr("Failed to load object models. Object models path does not exist."));
        return objectModels;
    } else if (!info.isDir()) {
        Q_EMIT error(tr("Failed to load object models. Object models path is not a folder."));
        return objectModels;
    }

    QDirIterator it(m_objectModelsPath, OBJECT_MODEL_FILES_EXTENSIONS, QDir::Files, QDirIterator::Subdirectories);
    int index = 0;
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        // We store only the filename as object model path, because that's
        // the format of the ground truth file used by the neural network
        ObjectModelPtr objectModel(new ObjectModel(QString::number(index),
                                                   fileInfo.fileName(),
                                                   fileInfo.absolutePath()));
        objectModels.append(objectModel);
        index++;
    }

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(
        objectModels.begin(),
        objectModels.end(),
        [&collator](ObjectModelPtr o1, ObjectModelPtr o2)
        {
            return collator.compare(o1->path(), o2->path()) < 0;
        });

    return objectModels;
}

QMap<QString, ImagePtr> createImageMap(const QList<ImagePtr> &images) {
    QMap<QString, ImagePtr> imageMap;

    for (int i = 0; i < images.size(); i++) {
        imageMap[images.at(i)->imagePath()] = images.at(i);
    }

    return imageMap;
}

QMap<QString, ObjectModelPtr> createObjectModelMap(const QList<ObjectModelPtr> &objectModels) {
    QMap<QString, ObjectModelPtr> objectModelMap;

    for (int i = 0; i < objectModels.size(); i++) {
        objectModelMap[objectModels.at(i)->path()] = objectModels.at(i);
    }

    return objectModelMap;
}

QList<PosePtr> JsonLoadAndStoreStrategy::loadPoses(const QList<ImagePtr> &images,
                                                     const QList<ObjectModelPtr> &objectModels) {
    QList<PosePtr> poses;
    m_posesWithInvalidData.clear();

    if (m_posesFilePath == Global::NO_PATH) {
        // The only time when the poses file path can be equal to the NO_PATH is
        // when the program is first started -> then we show a hint in the
        // breaadcrumbs to select an actual path, i.e. not a problem that we
        // return early here
        return poses;
    }

    //! See loadImages for why we don't throw an exception here
    if (!QFileInfo(m_posesFilePath).exists()) {
        Q_EMIT error(tr("Failed to load poses. Poses path does not exist."));
        return poses;
    }

    bool foundPosesWithInvalidPosesData = false;

    QFile jsonFile(m_posesFilePath);
    if (jsonFile.open(QFile::ReadWrite)) {
        QMap<QString, ImagePtr> imageMap = createImageMap(images);
        QMap<QString, ObjectModelPtr> objectModelMap = createObjectModelMap(objectModels);
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        if (jsonDocument.isNull()) {
            QJsonObject jsonObject = jsonDocument.object();
            //! If we need to update missing IDs we have to write back the document
            bool documentDirty = false;
            for(const QString& imagePath : jsonObject.keys()) {
                QJsonArray entriesForImage = jsonObject[imagePath].toArray();
                //! Index to keep track of entries to be able to update the
                //! poses' IDs if necessary. See reason to update the IDs
                //! further below.
                int index = 0;
                for(const QJsonValue &poseEntryRaw : entriesForImage) {
                    QJsonObject poseEntry = poseEntryRaw.toObject();
                    if (!poseEntry.contains("R") ||
                        !poseEntry.contains("t") ||
                        !poseEntry.contains("obj")) {
                        foundPosesWithInvalidPosesData = true;
                        continue;
                    }

                    //! Read rotation vector from json file
                    QJsonArray jsonRotationMatrix = poseEntry["R"].toArray();
                    QMatrix3x3 rotationMatrix = rotVectorFromJsonRotMatrix(jsonRotationMatrix);

                    QJsonArray translation = poseEntry["t"].toArray();
                    QVector3D qtTranslationVector = QVector3D((float) translation[0].toDouble(),
                                                              (float) translation[1].toDouble(),
                                                              (float) translation[2].toDouble());

                    QString objectModelPath = poseEntry["obj"].toString();
                    ImagePtr image = imageMap.value(imagePath);
                    ObjectModelPtr objectModel = objectModelMap.value(objectModelPath);

                    if (image && objectModel) {
                        //! If either is NULL, we do not manage the image or object model
                        //! specified in the JSON file, that's why we just skip the entry
                        //!
                        QString id = "";
                        //! An external ground truth file (e.g. from TLESS) might not have
                        //! IDs of exisiting poses. We need IDs to be able to
                        //! modify poses but if we are not the creator of the
                        //! pose we thus have to add an ID.
                        if (poseEntry.contains("id")) {
                            id = poseEntry["id"].toString();
                        } else {
                            id = GeneralHelper::createPoseId(*image, *objectModel);
                            //! No ID attatched to the entry yet -> write it to the file
                            //! to be able to identify the poses later
                            QJsonObject modifiedEntry(poseEntry);
                            modifiedEntry["id"] = id;
                            entriesForImage.replace(index, modifiedEntry);
                            jsonObject[imagePath] = entriesForImage;
                            documentDirty = true;
                        }

                        PosePtr pose(new Pose(id,
                                              qtTranslationVector,
                                              rotationMatrix,
                                              image,
                                              objectModel));
                        poses.append(pose);
                    }
                    index++;
                }
            }

            if (documentDirty) {
                //! We want to replace the old content, i.e. seek to 0
                jsonFile.resize(0);
                jsonFile.write(QJsonDocument(jsonObject).toJson());
            }
        } else {
            // TODO Json document is null, ie there was an error
        }
    } else {
        Q_EMIT error(tr("Failed to load poses. Poses file is not readable or writable."));
    }

    if (foundPosesWithInvalidPosesData) {
        Q_EMIT error(tr("There were poses with invalid data."));
    }

    return poses;
}
