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
        // JSON Document is null, an nerror occured, we cannot persist the pose
        return false;
    }

    QJsonObject jsonObject = jsonDocument.object();

    QString imagePath = objectImagePose.image().imagePath();
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
            entry["obj"] = objectImagePose.objectModel().path();
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
            newEntry["obj"] = objectImagePose.objectModel().path();
            newEntry["t"] = positionVectorArray;
            newEntry["R"] = rotationMatrixArray;
            entriesForImage << newEntry;
        }
    }
    m_ignorePosesFileChanged = true;
    jsonObject[imagePath] = entriesForImage;
    jsonFile.resize(0);
    jsonFile.write(QJsonDocument(jsonObject).toJson());

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

static Image createImageWithJsonParams(const QString &id,
                                       const QString& filename,
                                       const QString &segmentationFilename,
                                       const QString &imagesPath,
                                       float defaultNearPlane,
                                       float defaultFarPlane,
                                       QJsonObject &json) {
    QJsonObject parameters = json[filename].toObject();
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
    float nearPlane = defaultNearPlane;
    float farPlane = defaultFarPlane;
    if (parameters.contains("nearPlane")) {
        nearPlane = (float) parameters["nearPlane"].toDouble();
    }
    if (parameters.contains("farPlane")) {
        farPlane = (float) parameters["farPlane"].toDouble();
    }
    return Image(id, filename, segmentationFilename,
                 imagesPath, qtCameraMatrix,
                 nearPlane, farPlane);
}

QList<Image> JsonLoadAndStoreStrategy::loadImages() {
    QList<Image> images;
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
        Q_EMIT error(tr("Failed to load images. Images dir does not exist."));
        return images;
    } else if (!QFileInfo(m_imagesPath).isDir()) {
        Q_EMIT error(tr("Failed to load images. Images dir is not a folder."));
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

    if (imageFiles.size() == 0) {
        Q_EMIT error(tr("No images found at images dir."));
        return images;
    } else if (!jsonFile.exists()) {
        Q_EMIT error(tr("Failed to load images. Camera info file info.json does not exist."));
        return images;
    } else if (!jsonFile.open(QFile::ReadOnly)) {
        Q_EMIT error(tr("Failed to load images. Camera info file info.json is not readable."));
        return images;
    }

    QByteArray data = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
    if (jsonDocument.isNull()) {
        Q_EMIT error(tr("Failed to load images. Camera info file info.json is not a JSON file."));
        return images;
    }

    QJsonObject jsonObject = jsonDocument.object();
    // The user can define the near and far plane per image or
    // on a global level which will be used in case no individual
    // near and far plane are set on the image
    float nearPlane = NEAR_PLANE;
    if (jsonObject.contains("nearPlane")) {
        nearPlane = (float) jsonObject["nearPlane"].toDouble();
    }
    float farPlane = FAR_PLANE;
    if (jsonObject.contains("farPlane")) {
        farPlane = (float) jsonObject["farPlane"].toDouble();
    }
    for (int i = 0; i < imageFiles.size(); i ++) {
        QString image = imageFiles[i];
        QString imageFilename = QFileInfo(image).fileName();
        // If we don't have the camera matrix present in the cam info file skip this one
        if (!jsonObject[imageFilename].toObject().contains("K")) {
            // This can only happen when the camera matrix is invalid
            foundImageWithInvalidCameraMatrix = true;
            m_imagesWithInvalidData.append(imageFilename);
            continue;
        }
        Image newImage;
        if (segmentationImagesPathSet) {
            if (i > segmentationImageFiles.size() - 1) {
                // Nothing to do in this case, we don't know how to match images
                // and segmentation images
                Q_EMIT error(tr("Failed to load images. Number of segmentation images "
                                "does not match number of images."));
                return QList<Image>();
            }
            QString segmentationImageFile = segmentationImageFiles[i];
            QString segmentationImageFilePath =
                    QDir(m_segmentationImagesPath).absoluteFilePath(segmentationImageFile);
            newImage = createImageWithJsonParams(QString::number(i),
                                                 imageFilename,
                                                 segmentationImageFilePath,
                                                 m_imagesPath,
                                                 nearPlane,
                                                 farPlane,
                                                 jsonObject);
        } else {
            newImage = createImageWithJsonParams(QString::number(i),
                                                 imageFilename,
                                                 "",
                                                 m_imagesPath,
                                                 nearPlane,
                                                 farPlane,
                                                 jsonObject);
        }
        images.push_back(newImage);
    }

    if (foundImageWithInvalidCameraMatrix) {
        Q_EMIT error(tr("There were images with invalid camera matrices."));
    }

    if (images.size() == 0) {
        Q_EMIT error(tr("No images loaded (maybe because "
                        "camera matrix contains only invalid entries."));
        return images;
    }
    jsonFile.close();
    return images;
}

QList<ObjectModel> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModel> objectModels;

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
        ObjectModel objectModel(QString::number(index),
                                fileInfo.fileName(),
                                fileInfo.absolutePath());
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

    if (objectModels.size() == 0) {
        Q_EMIT error(tr("No object models found at object models dir."));
    }
    return objectModels;
}

QMap<QString, Image> createImageMap(const QList<Image> &images) {
    QMap<QString, Image> imageMap;

    for (int i = 0; i < images.size(); i++) {
        imageMap[images.at(i).imagePath()] = images.at(i);
    }

    return imageMap;
}

QMap<QString, ObjectModel> createObjectModelMap(const QList<ObjectModel> &objectModels) {
    QMap<QString, ObjectModel> objectModelMap;

    for (int i = 0; i < objectModels.size(); i++) {
        objectModelMap[objectModels.at(i).path()] = objectModels.at(i);
    }

    return objectModelMap;
}

QList<Pose> JsonLoadAndStoreStrategy::loadPoses(const QList<Image> &images,
                                                const QList<ObjectModel> &objectModels) {
    QList<Pose> poses;
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
        Q_EMIT error(tr("Failed to load poses. Poses file does not exist."));
        return poses;
    }

    bool foundPosesWithInvalidPosesData = false;

    QFile jsonFile(m_posesFilePath);
    if (!jsonFile.open(QFile::ReadWrite)) {
        Q_EMIT error(tr("Failed to load poses. Poses file is not readable or writable."));
        return poses;
    }

    QMap<QString, Image> imageMap = createImageMap(images);
    QMap<QString, ObjectModel> objectModelMap = createObjectModelMap(objectModels);
    QByteArray data = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data));

    if (jsonDocument.isNull()) {
        Q_EMIT error(tr("Failed to load poses. The poses file is not a JSON document."));
        return poses;
    }

    QJsonObject jsonObject = jsonDocument.object();
    //! If we need to update missing IDs we have to write back the document
    bool documentDirty = false;
    for(const QString& imagePath : jsonObject.keys()) {
        if (!jsonObject[imagePath].isArray()) {
            Q_EMIT error(tr("The JSON poses file does not contain an array of image entries."));
            return poses;
        }
        QJsonArray entriesForImage = jsonObject[imagePath].toArray();
        //! Index to keep track of entries to be able to update the
        //! poses' IDs if necessary. See reason to update the IDs
        //! further below.
        int index = 0;
        for(const QJsonValue &poseEntryRaw : entriesForImage) {
            QJsonObject poseEntry = poseEntryRaw.toObject();
            bool valuesValid = true;

            if (!poseEntry.contains("R") ||
                !poseEntry.contains("t") ||
                !poseEntry.contains("obj")) {
                valuesValid = false;
            }

            if (!poseEntry["R"].isArray()) {
                valuesValid = false;
            }

            if (!poseEntry["t"].isArray()) {
                valuesValid = false;
            }

            if (!poseEntry["obj"].isString()) {
                valuesValid = false;
            }

            if (!imageMap.contains(imagePath)) {
                valuesValid = false;
            }

            QString objectModelPath = poseEntry["obj"].toString();

            if (!objectModelMap.contains(objectModelPath)) {
                valuesValid = false;
            }

            QJsonArray jsonRotationMatrix = poseEntry["R"].toArray();
            QMatrix3x3 rotationMatrix = rotVectorFromJsonRotMatrix(jsonRotationMatrix);

            QJsonArray translation = poseEntry["t"].toArray();
            QVector3D qtTranslationVector = QVector3D((float) translation[0].toDouble(),
                                                      (float) translation[1].toDouble(),
                                                      (float) translation[2].toDouble());

            QString id = "none";
            if (poseEntry.contains("id")) {
                id = poseEntry["id"].toString();
            }

            if (!imageMap.contains(imagePath) ||
                    !objectModelMap.contains(objectModelPath) ||
                    !valuesValid) {
                m_posesWithInvalidData.append(id);
                continue;
            }

            Image image = imageMap.value(imagePath);
            ObjectModel objectModel = objectModelMap.value(objectModelPath);

            //! An external ground truth file (e.g. from TLESS) might not have
            //! IDs of exisiting poses. We need IDs to be able to
            //! modify poses but if we are not the creator of the
            //! pose we thus have to add an ID.
            if (id == "none") {
                id = GeneralHelper::createPoseId(image, objectModel);
                //! No ID attatched to the entry yet -> write it to the file
                //! to be able to identify the poses later
                QJsonObject modifiedEntry(poseEntry);
                modifiedEntry["id"] = id;
                entriesForImage.replace(index, modifiedEntry);
                jsonObject[imagePath] = entriesForImage;
                documentDirty = true;
            }

            Pose pose(id,
                      qtTranslationVector,
                      rotationMatrix,
                      image,
                      objectModel);
            poses.append(pose);

            index++;
        }
        if (documentDirty) {
            //! We want to replace the old content, i.e. seek to 0
            jsonFile.resize(0);
            jsonFile.write(QJsonDocument(jsonObject).toJson());
        }
    }

    if (foundPosesWithInvalidPosesData) {
        Q_EMIT error(tr("There were poses with invalid data."));
        // Invalid poses can be accessed through the strategy's public accessors
        qDebug() << "Poses with invalid data: ";
        qDebug() << m_posesWithInvalidData;
    }

    if (poses.size() == 0) {
        // Nothing to do here, maybe the file is empty
        qDebug() << "No poses loaded";
    }
    return poses;
}
