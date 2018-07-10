#include "jsonloadandstorestrategy.hpp"
#include "misc/generalhelper.h"

#include <opencv2/core/mat.hpp>

#include <QDirIterator>
#include <QCollator>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

const QStringList JsonLoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS =
                                            QStringList({"*.obj", "*.ply", "*.3ds", "*.fbx"});

static QString convertPathToSuffxFileName(const QString &pathToConvert,
                                          const QString &suffix,
                                          const QString &extension) {
    return QFileInfo(pathToConvert).completeBaseName() + suffix + extension;
}

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy() {
    connectWatcherSignals();
}

JsonLoadAndStoreStrategy::JsonLoadAndStoreStrategy(const QDir &imagesPath, const QDir &objectModelsPath,
                                                           const QDir &correspondencesFilePath)
    : imagesPath(imagesPath),
      objectModelsPath(objectModelsPath),
      correspondencesFilePath(correspondencesFilePath) {
    watcher.addPaths(QStringList() << imagesPath.path()
                                   << objectModelsPath.path()
                                   << correspondencesFilePath.path());
    connectWatcherSignals();
}

JsonLoadAndStoreStrategy::~JsonLoadAndStoreStrategy() {
}

bool JsonLoadAndStoreStrategy::persistObjectImageCorrespondence(
        Correspondence *objectImageCorrespondence, bool deleteCorrespondence) {
    // no need to check whether paths exist because setters do so already

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!QFileInfo(correspondencesFilePath.path()).exists()) {
        Q_EMIT failedToPersistCorrespondence("The specified JSON file does not exist.");
        return false;
    }

    //! Read in the camera parameters from the JSON file
    QFile jsonFile(correspondencesFilePath.path());
    if (jsonFile.open(QFile::ReadWrite)) {
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();

        QString imagePath = objectImageCorrespondence->getImage()->getImagePath();
        QJsonArray entriesForImage;

        if (deleteCorrespondence) {
            if (jsonObject.contains(imagePath)) {
                entriesForImage = jsonObject[imagePath].toArray();
                int index = 0;
                for(const QJsonValue &entry : entriesForImage) {
                    QJsonObject entryObject = entry.toObject();
                    if (entryObject["id"] == objectImageCorrespondence->getID()) {
                        entriesForImage.removeAt(index);
                    }
                    index++;
                }
                jsonObject[imagePath] = entriesForImage;
            }
        } else {
            //! Preparation of 3D data for the JSON file
            QMatrix3x3 rotationMatrix = objectImageCorrespondence->getRotation();
            QJsonArray rotationMatrixArray;
            rotationMatrixArray << rotationMatrix(0, 0) << rotationMatrix(0, 1) << rotationMatrix(0, 2)
                                << rotationMatrix(1, 0) << rotationMatrix(1, 1) << rotationMatrix(1, 2)
                                << rotationMatrix(2, 0) << rotationMatrix(2, 1) << rotationMatrix(2, 2);
            QVector3D positionVector = objectImageCorrespondence->getPosition();
            QJsonArray positionVectorArray;
            positionVectorArray << positionVector[0] << positionVector[1] << positionVector[2];
            //! Check if any entries for the image exist
            if (jsonObject.contains(imagePath)) {
                //! There are some entries already and we check whether the correspondence
                //! already exists
                entriesForImage = jsonObject[imagePath].toArray();
                //! We have to check whether our correspondence exists, and if it does, only update it
                //! If we don't find it we have to create it anew and add it to the list of correspondences
                bool entryFound = false;
                //! Keep track of the index if we find an existing correspondence
                int index = 0;
                //! Create new entry object, as we can't modify the exisiting ones directly somehow
                QJsonObject entry;
                entry["id"] = objectImageCorrespondence->getID();
                entry["obj"] = objectImageCorrespondence->getObjectModel()->getPath();
                entry["R"] = rotationMatrixArray;
                entry["t"] = positionVectorArray;

                for(const QJsonValue &_entry : entriesForImage) {
                    QJsonObject entryObject = _entry.toObject();
                    if (entryObject["id"] == objectImageCorrespondence->getID()) {
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
                newEntry["id"] = objectImageCorrespondence->getID();
                newEntry["obj"] = objectImageCorrespondence->getObjectModel()->getPath();
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
        Q_EMIT failedToPersistCorrespondence("Could not read the specified JSON file.");
        return false;
    }
}

static QMatrix3x3 rotVectorFromJsonRotMatrix(QJsonArray &jsonRotationMatrix) {
    QMatrix3x3 rotationMatrix = QMatrix3x3(new float[9]{
       (float) jsonRotationMatrix[0].toDouble(),
       (float) jsonRotationMatrix[1].toDouble(),
       (float) jsonRotationMatrix[2].toDouble(),
       (float) jsonRotationMatrix[3].toDouble(),
       (float) jsonRotationMatrix[4].toDouble(),
       (float) jsonRotationMatrix[5].toDouble(),
       (float) jsonRotationMatrix[6].toDouble(),
       (float) jsonRotationMatrix[7].toDouble(),
       (float) jsonRotationMatrix[8].toDouble()});
    return rotationMatrix;
}

static Image createImageWithJsonParams(const QString& filename, const QString &segmentationFilename,
                                       const QString &imagesPath, QJsonObject &json) {
    QJsonObject parameters = json[filename].toObject();
    QJsonArray cameraMatrix = parameters["K"].toArray();
    QMatrix3x3 qtCameraMatrix = QMatrix3x3(new float[9]{
                (float) cameraMatrix[0].toDouble(),
                (float) cameraMatrix[1].toDouble(),
                (float) cameraMatrix[2].toDouble(),
                (float) cameraMatrix[3].toDouble(),
                (float) cameraMatrix[4].toDouble(),
                (float) cameraMatrix[5].toDouble(),
                (float) cameraMatrix[6].toDouble(),
                (float) cameraMatrix[7].toDouble(),
                (float) cameraMatrix[8].toDouble()});
    return Image(filename, segmentationFilename, imagesPath, qtCameraMatrix);
}

QList<Image> JsonLoadAndStoreStrategy::loadImages() {
    QList<Image> images;

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!pathExists(imagesPath)) {
        Q_EMIT failedToLoadImages("The specified path does not exist.");
        return images;
    }

    QStringList fileFilter("*" + imageFilesExtension);
    QStringList files = imagesPath.entryList(fileFilter, QDir::Files, QDir::Name);

    //! Read in the camera parameters from the JSON file
    QFile jsonFile(imagesPath.filePath("info.json"));
    if (files.size() > 0 && jsonFile.open(QFile::ReadOnly)) {
        QByteArray data = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDocument.object();
        for (int i = 0; i < files.size(); i += 2) {
            QString image = files[i];
            QString imageFilename = QFileInfo(image).fileName();
            //! Check the next image if it is the segmentation image of the currently inspected image
            int j = i + 1;

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
                    images.push_back(createImageWithJsonParams(imageFilename,
                                                               imagesPath.path(),
                                                               segmentationImageFilename,
                                                               jsonObject));
                } else {
                    //! We didn't find a segmentation image, i.e. two separate images have to be added
                    //! Setting the segmentation image to "" is essentially what the constructor of
                    //! Image does, i.e. we do not need to duplicate the create Image method to
                    //! obtain an Image without segmentation image
                    images.push_back(createImageWithJsonParams(imageFilename,
                                                               imagesPath.path(),
                                                               "",
                                                               jsonObject));
                    //! If two different images and not image and segmentation image we also have to load the appropriate
                    //! camera parameters
                    images.push_back(createImageWithJsonParams(secondImageFilename,
                                                               imagesPath.path(),
                                                               "",
                                                               jsonObject));
                }
            } else {
                images.push_back(createImageWithJsonParams(imageFilename,
                                                           imagesPath.path(),
                                                           "",
                                                           jsonObject));
            }
        }
    } else if (files.size() > 0) {
        //! Only if we can read images but do not find the JSON info file we raise the exception
        Q_EMIT failedToLoadImages("Could not find info.json with the camera parameters.");
    } else if (files.size() == 0) {
        Q_EMIT failedToLoadImages("No images found at the specified path.");
    }

    return images;
}

QList<ObjectModel> JsonLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModel> objectModels;

    //! See explanation under loadImages for why we don't throw an exception here
    if (!pathExists(objectModelsPath.path())) {
        Q_EMIT failedToLoadObjectModels("The specified path does not exist.");
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

QList<Correspondence> JsonLoadAndStoreStrategy::loadCorrespondences(const QList<Image> &images, const QList<ObjectModel> &objectModels) {
    QList<Correspondence> correspondences;

    //! See loadImages for why we don't throw an exception here
    if (!QFileInfo(correspondencesFilePath.path()).exists()) {
        Q_EMIT failedToLoadCorrespondences("The specified path does not exist.");
        return correspondences;
    }

    QFile jsonFile(correspondencesFilePath.path());
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
            //! correspondences' IDs if necessary. See reason to update the IDs
            //! further below.
            int index = 0;
            for(const QJsonValue &correspondenceEntryRaw : entriesForImage) {
                QJsonObject correspondenceEntry = correspondenceEntryRaw.toObject();
                Q_ASSERT(correspondenceEntry.contains("R"));
                Q_ASSERT(correspondenceEntry.contains("t"));
                Q_ASSERT(correspondenceEntry.contains("obj"));

                //! Read rotation vector from json file
                QJsonArray jsonRotationMatrix = correspondenceEntry["R"].toArray();
                QMatrix3x3 rotationMatrix = rotVectorFromJsonRotMatrix(jsonRotationMatrix);

                QJsonArray translation = correspondenceEntry["t"].toArray();
                QVector3D qtTranslationVector = QVector3D((float) translation[0].toDouble(),
                                                          (float) translation[1].toDouble(),
                                                          (float) translation[2].toDouble());

                QString objectModelPath = correspondenceEntry["obj"].toString();
                const Image *image = imageMap.value(imagePath);
                const ObjectModel *objectModel = objectModelMap.value(objectModelPath);

                if (image && objectModel) {
                    //! If either is NULL, we do not manage the image or object model
                    //! specified in the JSON file, that's why we just skip the entry
                    //!
                    QString id = "";
                    //! An external ground truth file (e.g. from TLESS) might not have
                    //! IDs of exisiting correspondences. We need IDs to be able to
                    //! modify correspondences but if we are not the creator of the
                    //! correspondence we thus have to add an ID.
                    if (correspondenceEntry.contains("id")) {
                        id = correspondenceEntry["id"].toString();
                    } else {
                        id = GeneralHelper::createCorrespondenceId(image, objectModel);
                        //! No ID attatched to the entry yet -> write it to the file
                        //! to be able to identify the correspondences later
                        QJsonObject modifiedEntry(correspondenceEntry);
                        modifiedEntry["id"] = id;
                        entriesForImage.replace(index, modifiedEntry);
                        jsonObject[imagePath] = entriesForImage;
                        documentDirty = true;
                    }

                    correspondences.append(Correspondence(id,
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

    return correspondences;
}

bool JsonLoadAndStoreStrategy::setImagesPath(const QDir &path) {
    if (!pathExists(path))
        return false;
    if (imagesPath == path)
        return true;

    watcher.removePath(imagesPath.path());
    watcher.addPath(path.path());
    imagesPath = path;

    Q_EMIT imagesChanged();

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

    watcher.removePath(objectModelsPath.path());
    watcher.addPath(path.path());
    objectModelsPath = path;

    Q_EMIT objectModelsChanged();

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

    watcher.removePath(correspondencesFilePath.path());
    watcher.addPath(path.path());
    correspondencesFilePath = path;

    Q_EMIT correspondencesChanged();

    return true;
}

QDir JsonLoadAndStoreStrategy::getCorrespondencesFilePath() const {
    return correspondencesFilePath.path();
}

void JsonLoadAndStoreStrategy::setSegmentationImageFilesSuffix(const QString &suffix) {
    //! Only set suffix if it differs from the suffix before because we then have to reload images
    if (segmentationImageFilesSuffix.compare(suffix) != 0) {
        segmentationImageFilesSuffix = suffix;
        Q_EMIT imagesChanged();
    }
}

QString JsonLoadAndStoreStrategy::getSegmentationImageFilesSuffix() {
    return segmentationImageFilesSuffix;
}

void JsonLoadAndStoreStrategy::setImageFilesExtension(const QString &extension) {
    if (extension.compare("") && imageFilesExtension.compare(extension) != 0) {
        imageFilesExtension = extension;
        Q_EMIT imagesChanged();
    }
}

QString JsonLoadAndStoreStrategy::getImageFilesExtension() {
    return imageFilesExtension;
}

void JsonLoadAndStoreStrategy::onDirectoryChanged(const QString &path) {
    if (path == imagesPath.path()) {
        Q_EMIT imagesChanged();
    } else if (path == objectModelsPath.path()) {
        Q_EMIT objectModelsChanged();
    } else if (path == correspondencesFilePath.path()) {
        Q_EMIT correspondencesChanged();
    }
}

void JsonLoadAndStoreStrategy::onFileChanged(const QString &filePath) {
    // Only for images and object models, because storing correspondences
    // at the correspondence file path will trigger this signal as well,
    // but we already updated the program accordingly (of course)
    if (filePath == correspondencesFilePath.path()) {
        Q_EMIT correspondencesChanged();
    } else if (filePath.contains(imagesPath.path()) && filePath.endsWith(imageFilesExtension)) {
        Q_EMIT imagesChanged();
    } else if (filePath.contains(objectModelsPath.path())
               && OBJECT_MODEL_FILES_EXTENSIONS.contains(filePath.right(4))) {
        Q_EMIT objectModelsChanged();
    }
}

void JsonLoadAndStoreStrategy::connectWatcherSignals() {
    connect(&watcher, &QFileSystemWatcher::directoryChanged,
            this, &JsonLoadAndStoreStrategy::onDirectoryChanged);
    connect(&watcher, &QFileSystemWatcher::fileChanged,
            this, &JsonLoadAndStoreStrategy::onFileChanged);
}
