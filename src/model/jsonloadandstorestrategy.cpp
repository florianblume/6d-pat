#include "jsonloadandstorestrategy.hpp"
#include "misc/generalhelper.hpp"

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

const QStringList JsonLoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS =
                                            QStringList({"*.obj", "*.ply", "*.3ds", "*.fbx"});
const QStringList JsonLoadAndStoreStrategy::IMAGE_FILES_EXTENSIONS =
                                            QStringList({"*.jpg", "*.jpeg", "*.png", "*.tiff"});

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy()  {
    connectWatcherSignals();
}

JsonLoadAndStoreStrategy::~JsonLoadAndStoreStrategy() {
}

bool JsonLoadAndStoreStrategy::persistPose(const Pose &objectImagePose, bool deletePose) {

    // Read in the camera parameters from the JSON file
    QFileInfo info(posesFilePath);
    QFile jsonFile(posesFilePath);

    if (!info.isFile()) {
        Q_EMIT error(FailedToPersistPosePosesPathIsNotAFile);
        return false;
    }

    if (!jsonFile.open(QFile::ReadWrite)) {
        Q_EMIT error(FailedToPersistPosePosesFileCouldNotBeRead);
        return false;
    }

    QByteArray data = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
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
    ignorePosesFileChanged = true;
    jsonObject[imagePath] = entriesForImage;
    jsonFile.resize(0);
    jsonFile.write(QJsonDocument(jsonObject).toJson());
    return true;
}

void JsonLoadAndStoreStrategy::setImagesPath(const QString &imagesPath) {
    setPath(imagesPath, this->imagesPath);
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
    return ImagePtr(new Image(filename, segmentationFilename, imagesPath, qtCameraMatrix));
}

QList<ImagePtr> JsonLoadAndStoreStrategy::loadImages() {
    QList<ImagePtr> images;

    // we do not need to throw an exception here, the only time the path cannot exist
    // is if this strategy was constructed with an empty path, all other methods of
    // setting the path check if the path exists
    if (!QFileInfo(imagesPath).exists()) {
        emit error(ImagesPathDoesNotExist);
        return images;
    } else if (segmentationImagesPath != "" && !QFileInfo(segmentationImagesPath).exists()) {
        emit error(SegmentationImagesPathDoesNotExist);
        return images;
    }

    QStringList imageFiles = QDir(imagesPath).entryList(IMAGE_FILES_EXTENSIONS, QDir::Files, QDir::Name);
    QStringList segmentationImageFiles =
            QDir(segmentationImagesPath).entryList(IMAGE_FILES_EXTENSIONS, QDir::Files, QDir::Name);
    // Also ensure that the number of elements is the same
    bool segmentationImagesPathSet = segmentationImagesPath != ""
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
    QFile jsonFile(QDir(imagesPath).filePath("info.json"));

    if (imageFiles.size() > 0 && jsonFile.open(QFile::ReadOnly)) {
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();
        for (int i = 0; i < imageFiles.size(); i ++) {
            QString image = imageFiles[i];
            QString imageFilename = QFileInfo(image).fileName();
            if (segmentationImagesPathSet) {
                QString segmentationImageFile = segmentationImageFiles[i];
                QString segmentationImageFilePath =
                        QDir(segmentationImagesPath).absoluteFilePath(segmentationImageFile);
                images.push_back(createImageWithJsonParams(imageFilename,
                                                           segmentationImageFilePath,
                                                           imagesPath,
                                                           jsonObject));
            } else {
                images.push_back(createImageWithJsonParams(imageFilename,
                                                           "",
                                                           imagesPath,
                                                           jsonObject));
            }
        }
    } else if (imageFiles.size() > 0) {
        // Only if we can read images but do not find the JSON info file we raise the exception
        Q_EMIT error(CamInfoDoesNotExist);
    } else if (imageFiles.size() == 0) {
        Q_EMIT error(NoImagesFound);
    }

    return images;
}

void JsonLoadAndStoreStrategy::setObjectModelsPath(const QString &objectModelsPath) {
    setPath(objectModelsPath, this->objectModelsPath);

}

QList<ObjectModelPtr> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModelPtr> objectModels;

    // See explanation under loadImages for why we don't throw an exception here
    QFileInfo info(objectModelsPath);
    if (!info.exists()) {
        Q_EMIT error(ObjectModelsPathDoesNotExist);
        return objectModels;
    } else if (!info.isDir()) {
        Q_EMIT error(ObjectModelsPathIsNotAFolder);
        return objectModels;
    }

    QDirIterator it(objectModelsPath, OBJECT_MODEL_FILES_EXTENSIONS, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        // We store only the filename as object model path, because that's
        // the format of the ground truth file used by the neural network
        ObjectModelPtr objectModel(new ObjectModel(fileInfo.fileName(), fileInfo.absolutePath()));
        objectModels.append(objectModel);
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

void JsonLoadAndStoreStrategy::setPosesFilePath(const QString &posesFilePath) {
    setPath(posesFilePath, this->posesFilePath);
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

    //! See loadImages for why we don't throw an exception here
    if (!QFileInfo(posesFilePath).exists()) {
        Q_EMIT error(PosesPathDoesNotExist);
        return poses;
    }

    QFile jsonFile(posesFilePath);
    if (jsonFile.open(QFile::ReadWrite)) {
        QMap<QString, ImagePtr> imageMap = createImageMap(images);
        QMap<QString, ObjectModelPtr> objectModelMap = createObjectModelMap(objectModels);
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
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
                Q_ASSERT(poseEntry.contains("R"));
                Q_ASSERT(poseEntry.contains("t"));
                Q_ASSERT(poseEntry.contains("obj"));

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
        Q_EMIT error(PosesPathIsNotReadable);
    }

    return poses;
}

void JsonLoadAndStoreStrategy::onSettingsChanged(SettingsPtr settings) {
    if (settings->imagesPath() != imagesPath) {
        setImagesPath(settings->imagesPath());
    }
    if (settings->segmentationImagesPath() != segmentationImagesPath) {
        setSegmentationImagesPath(settings->segmentationImagesPath());
    }
    if (settings->objectModelsPath() != objectModelsPath) {
        setObjectModelsPath(settings->objectModelsPath());
    }
    if (settings->posesFilePath() != posesFilePath) {
        setPosesFilePath(settings->posesFilePath());
    }
}

bool JsonLoadAndStoreStrategy::setPath(const QString &path, QString &oldPath) {
    if (!QFileInfo(path).exists())
        return false;
    if (oldPath == path)
        return true;

    if (oldPath != "") {
        watcher.removePath(oldPath);
    }
    watcher.addPath(path);
    oldPath = path;

    return true;
}

void JsonLoadAndStoreStrategy::setSegmentationImagesPath(const QString &path) {
    //! Only set suffix if it differs from the suffix before because we then have to reload images
    if (segmentationImagesPath != path) {
        setPath(path, segmentationImagesPath);
    }
}

void JsonLoadAndStoreStrategy::onDirectoryChanged(const QString &path) {
    if (path == imagesPath || path == segmentationImagesPath) {
        Q_EMIT dataChanged(Data::Images);
    } else if (path == objectModelsPath) {
        Q_EMIT dataChanged(Data::ObjectModels);
    } else if (path == posesFilePath) {
        Q_EMIT dataChanged(Data::Poses);
    }
}

void JsonLoadAndStoreStrategy::onFileChanged(const QString &filePath) {
    // Only for images and object models, because storing poses
    // at the pose file path will trigger this signal as well,
    // but we already updated the program accordingly (of course)
    if (filePath == posesFilePath) {
        if (!ignorePosesFileChanged) {
            Q_EMIT dataChanged(Data::Poses);;
        }
        ignorePosesFileChanged = false;
    } else if (filePath.contains(imagesPath)
               && IMAGE_FILES_EXTENSIONS.contains(filePath.right(4))) {
        Q_EMIT dataChanged(Data::Images);
    } else if (filePath.contains(objectModelsPath)
               && OBJECT_MODEL_FILES_EXTENSIONS.contains(filePath.right(4))) {
        Q_EMIT dataChanged(Data::ObjectModels);
    }
}

void JsonLoadAndStoreStrategy::connectWatcherSignals() {
    connect(&watcher, &QFileSystemWatcher::directoryChanged,
            this, &JsonLoadAndStoreStrategy::onDirectoryChanged);
    connect(&watcher, &QFileSystemWatcher::fileChanged,
            this, &JsonLoadAndStoreStrategy::onFileChanged);
}
