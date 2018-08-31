#include "jsonloadandstorestrategy.hpp"
#include "misc/generalhelper.h"

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

const QStringList JsonLoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS =
                                            QStringList({"*.obj", "*.ply", "*.3ds", "*.fbx"});
const QStringList JsonLoadAndStoreStrategy::IMAGE_FILES_EXTENSIONS =
                                            QStringList({"*.jpg", "*.jpeg", "*.png", "*.tiff"});

static QString convertPathToSuffxFileName(const QString &pathToConvert,
                                          const QString &suffix,
                                          const QString &extension) {
    return QFileInfo(pathToConvert).completeBaseName() + suffix + extension;
}

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy(SettingsStore *settingsStore,
                                                   const QString settingsIdentifier) :
    LoadAndStoreStrategy(settingsStore, settingsIdentifier) {
    connectWatcherSignals();
    // Simply call settings changed to load the paths, etc
    onSettingsChanged(settingsIdentifier);
}

JsonLoadAndStoreStrategy::~JsonLoadAndStoreStrategy() {
}

bool JsonLoadAndStoreStrategy::persistPose(
        Pose *objectImagePose, bool deletePose) {

    //! Read in the camera parameters from the JSON file
    QFile jsonFile(posesFilePath);
    if (jsonFile.open(QFile::ReadWrite)) {
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();

        QString imagePath = objectImagePose->getImage()->getImagePath();
        QJsonArray entriesForImage;

        if (deletePose) {
            if (jsonObject.contains(imagePath)) {
                entriesForImage = jsonObject[imagePath].toArray();
                int index = 0;
                for(const QJsonValue &entry : entriesForImage) {
                    QJsonObject entryObject = entry.toObject();
                    if (entryObject["id"] == objectImagePose->getID()) {
                        entriesForImage.removeAt(index);
                    }
                    index++;
                }
                jsonObject[imagePath] = entriesForImage;
            }
        } else {
            //! Preparation of 3D data for the JSON file
            QMatrix3x3 rotationMatrix = objectImagePose->getRotation();
            QJsonArray rotationMatrixArray;
            rotationMatrixArray << rotationMatrix(0, 0) << rotationMatrix(0, 1) << rotationMatrix(0, 2)
                                << rotationMatrix(1, 0) << rotationMatrix(1, 1) << rotationMatrix(1, 2)
                                << rotationMatrix(2, 0) << rotationMatrix(2, 1) << rotationMatrix(2, 2);
            QVector3D positionVector = objectImagePose->getPosition();
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
                entry["id"] = objectImagePose->getID();
                entry["obj"] = objectImagePose->getObjectModel()->getPath();
                entry["R"] = rotationMatrixArray;
                entry["t"] = positionVectorArray;

                for(const QJsonValue &_entry : entriesForImage) {
                    QJsonObject entryObject = _entry.toObject();
                    if (entryObject["id"] == objectImagePose->getID()) {
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
                newEntry["id"] = objectImagePose->getID();
                newEntry["obj"] = objectImagePose->getObjectModel()->getPath();
                newEntry["t"] = positionVectorArray;
                newEntry["R"] = rotationMatrixArray;
                entriesForImage << newEntry;
            }
        }
        jsonObject[imagePath] = entriesForImage;
        jsonFile.resize(0);
        jsonFile.write(QJsonDocument(jsonObject).toJson());
        return true;
    } else {
        Q_EMIT failedToPersistPose("Could not read the specified JSON file.");
        return false;
    }
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

static Image createImageWithJsonParams(const QString& filename, const QString &segmentationFilename,
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
    return Image(filename, segmentationFilename, imagesPath, qtCameraMatrix);
}

QList<Image> JsonLoadAndStoreStrategy::loadImages() {
    QList<Image> images;

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!QFileInfo(imagesPath).exists()) {
        emit failedToLoadImages("The specified images path does not exist.");
        return images;
    } else if (segmentationImagesPath != "" && !QFileInfo(segmentationImagesPath).exists()) {
        emit failedToLoadImages("The specified segmentation images path does not exist.");
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

    //! Read in the camera parameters from the JSON file
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
        //! Only if we can read images but do not find the JSON info file we raise the exception
        Q_EMIT failedToLoadImages("Could not find info.json with the camera parameters.");
    } else if (imageFiles.size() == 0) {
        Q_EMIT failedToLoadImages("No images found at the specified path.");
    }

    return images;
}

QList<ObjectModel> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModel> objectModels;

    //! See explanation under loadImages for why we don't throw an exception here
    if (!QFileInfo(objectModelsPath).exists()) {
        Q_EMIT failedToLoadObjectModels("The specified path does not exist.");
        return objectModels;
    }

    QDirIterator it(objectModelsPath, OBJECT_MODEL_FILES_EXTENSIONS, QDir::Files, QDirIterator::Subdirectories);
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

QList<Pose> JsonLoadAndStoreStrategy::loadPoses(const QList<Image> &images, const QList<ObjectModel> &objectModels) {
    QList<Pose> poses;

    //! See loadImages for why we don't throw an exception here
    if (!QFileInfo(posesFilePath).exists()) {
        Q_EMIT failedToLoadPoses("The specified path does not exist.");
        return poses;
    }

    QFile jsonFile(posesFilePath);
    if (jsonFile.open(QFile::ReadWrite)) {
        QMap<QString, const Image*> imageMap = createImageMap(images);
        QMap<QString, const ObjectModel*> objectModelMap = createObjectModelMap(objectModels);
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
                const Image *image = imageMap.value(imagePath);
                const ObjectModel *objectModel = objectModelMap.value(objectModelPath);

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
                        id = GeneralHelper::createPoseId(image, objectModel);
                        //! No ID attatched to the entry yet -> write it to the file
                        //! to be able to identify the poses later
                        QJsonObject modifiedEntry(poseEntry);
                        modifiedEntry["id"] = id;
                        entriesForImage.replace(index, modifiedEntry);
                        jsonObject[imagePath] = entriesForImage;
                        documentDirty = true;
                    }

                    poses.append(Pose(id,
                                                                 qtTranslationVector,
                                                                 rotationMatrix,
                                                                 image,
                                                                 objectModel));
                }
                index++;
            }
        }

        if (documentDirty) {
            //! We want to replace the old content, i.e. seek to 0
            jsonFile.resize(0);
            jsonFile.write(QJsonDocument(jsonObject).toJson());
        }
    }

    return poses;
}

void JsonLoadAndStoreStrategy::onSettingsChanged(const QString settingsIdentifier) {
    QSharedPointer<Settings> settings
            = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    if (settings->getImagesPath() != imagesPath) {
        setImagesPath(settings->getImagesPath());
    }
    if (settings->getSegmentationImagesPath() != segmentationImagesPath) {
        setSegmentationImagesPath(settings->getSegmentationImagesPath());
    }
    if (settings->getObjectModelsPath() != objectModelsPath) {
        setObjectModelsPath(settings->getObjectModelsPath());
    }
    if (settings->getPosesFilePath() != posesFilePath) {
        setPosesFilePath(settings->getPosesFilePath());
    }
}

bool JsonLoadAndStoreStrategy::setImagesPath(const QString &path) {
    if (!QFileInfo(path).exists())
        return false;
    if (imagesPath == path)
        return true;

    watcher.removePath(imagesPath);
    watcher.addPath(path);
    imagesPath = path;

    Q_EMIT imagesChanged();

    return true;
}

bool JsonLoadAndStoreStrategy::setObjectModelsPath(const QString &path) {
    if (!QFileInfo(path).exists())
        return false;
    if (objectModelsPath == path)
        return true;

    watcher.removePath(objectModelsPath);
    watcher.addPath(path);
    objectModelsPath = path;

    Q_EMIT objectModelsChanged();

    return true;
}

bool JsonLoadAndStoreStrategy::setPosesFilePath(const QString &path) {
    if (!QFileInfo(path).exists())
        return false;
    if (posesFilePath == path)
        return true;

    watcher.removePath(posesFilePath);
    watcher.addPath(path);
    posesFilePath = path;

    Q_EMIT posesChanged();

    return true;
}

void JsonLoadAndStoreStrategy::setSegmentationImagesPath(const QString &path) {
    //! Only set suffix if it differs from the suffix before because we then have to reload images
    if (segmentationImagesPath != path) {
        segmentationImagesPath = path;
        Q_EMIT imagesChanged();
    }
}

void JsonLoadAndStoreStrategy::onDirectoryChanged(const QString &path) {
    if (path == imagesPath) {
        Q_EMIT imagesChanged();
    } else if (path == objectModelsPath) {
        Q_EMIT objectModelsChanged();
    } else if (path == posesFilePath) {
        Q_EMIT posesChanged();
    }
}

void JsonLoadAndStoreStrategy::onFileChanged(const QString &filePath) {
    // Only for images and object models, because storing poses
    // at the pose file path will trigger this signal as well,
    // but we already updated the program accordingly (of course)
    if (filePath == posesFilePath) {
        emit posesChanged();
    } else if (filePath.contains(imagesPath)
               && IMAGE_FILES_EXTENSIONS.contains(filePath.right(4))) {
        emit imagesChanged();
    } else if (filePath.contains(objectModelsPath)
               && OBJECT_MODEL_FILES_EXTENSIONS.contains(filePath.right(4))) {
        emit objectModelsChanged();
    }
}

void JsonLoadAndStoreStrategy::connectWatcherSignals() {
    connect(&watcher, &QFileSystemWatcher::directoryChanged,
            this, &JsonLoadAndStoreStrategy::onDirectoryChanged);
    connect(&watcher, &QFileSystemWatcher::fileChanged,
            this, &JsonLoadAndStoreStrategy::onFileChanged);
}
