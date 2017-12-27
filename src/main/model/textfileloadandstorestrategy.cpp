#include "textfileloadandstorestrategy.hpp"
#include <QDirIterator>
#include <QTextStream>
#include <QMap>

const QString TextFileLoadAndStoreStrategy::CORRESPONDENCE_FORMAT_DELIMITER = " ";
const QString TextFileLoadAndStoreStrategy::CORRESPONDENCE_FILES_NAME_SUFFIX = "_correspondence";
const QString TextFileLoadAndStoreStrategy::CORRESPONDENCE_FILES_EXTENSION = ".txt";
const QList<QString> TextFileLoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS = {".obj", ".ply"};

TextFileLoadAndStoreStrategy::TextFileLoadAndStoreStrategy() {
    // nothing to do here
}

TextFileLoadAndStoreStrategy::TextFileLoadAndStoreStrategy(const QDir &imagesPath, const QDir &objectModelsPath,
                                                           const QDir &correspondencesPath)
    : imagesPath(imagesPath), objectModelsPath(objectModelsPath), correspondencesPath(correspondencesPath) {
    if (!pathExists(imagesPath))
        throw "Image path does not exist";
    if (!pathExists(objectModelsPath))
        throw "ObjectModels path does not exist";
    if (!pathExists(correspondencesPath))
        throw "Correspondences path does not exist";
}

TextFileLoadAndStoreStrategy::~TextFileLoadAndStoreStrategy() {
}

static QString convertPathToSuffxFileName(const QString &pathToConvert, const QString &suffix, const QString &extension) {
    return QFileInfo(pathToConvert).completeBaseName() + suffix + extension;
}

static void persistCorrespondenceToFile(QFile &inFile, QFile &outFile,
                                        const ObjectImageCorrespondence& correspondence,
                                        bool deleteCorrespondence) {
    bool correspondenceFound = false;

    //! count how many lines are not this correspondence, if there aren't any
    //! other and this correspondence is to be deleted, remove the file
    uint foreignLinesCount = 0;

    QTextStream outStream(&outFile);

    QTextStream in(&inFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList splitLine = line.split(TextFileLoadAndStoreStrategy::CORRESPONDENCE_FORMAT_DELIMITER);
        //! Check if we found the correspondence that we want to update
        if (splitLine.size() > 0 && splitLine.at(0).compare(correspondence.getID()) == 0) {
            //! We did find it
            correspondenceFound = true;
            if (!deleteCorrespondence)
                //! if we are to delete the correspondence, simply don't write back the line
                outStream << correspondence.toString() << endl;
        } else {
                //! Different correspondence, no need to do anything just write the line back
                outStream << line << endl;
                foreignLinesCount++;
        }
    }

    if (!correspondenceFound && !deleteCorrespondence) {
        //! Since we didn't find the correspondence it must be a new one and we add it at the end of the file
        //! but only if the delete flag isn't set to true
        outStream << correspondence.toString();
    } else if (foreignLinesCount == 0 && deleteCorrespondence) {
        //! Remove the file if there is no data stored in it anymore
        outFile.remove();
        inFile.remove();
    }
}

bool TextFileLoadAndStoreStrategy::persistObjectImageCorrespondence(
        const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) {
    // no need to check whether paths exist because setters do so already

    //! we do not need to throw an exception here, the only time the path cannot exist
    //! is if this strategy was constructed with an empty path, all other methods of
    //! setting the path check if the path exists
    if (!pathExists(correspondencesPath)) {
        return false;
    }

    QDir image = objectImageCorrespondence.getImage()->getImagePath();
    //! Only the filename, e.g. img01_correspondence.txt
    QDir correspondenceFileName = convertPathToSuffxFileName(image.path(),
                                                             CORRESPONDENCE_FILES_NAME_SUFFIX,
                                                             CORRESPONDENCE_FILES_EXTENSION);
    //! The full path to the correspondence file, e.g. /home/user/documents/img01_correspondence.txt
    QDir correspondenceFilePath = correspondencesPath.filePath(correspondenceFileName.path());
    QFileInfo correspondenceFileInfo(correspondenceFilePath.path());
    if (correspondenceFileInfo.exists() && correspondenceFileInfo.isFile()) {
        QDir temp("./tmp");
        QFile tempFile(temp.path());
        QFile correspondenceFile(correspondenceFilePath.path());
        if (tempFile.exists()) {
            tempFile.remove();
        }
        if (tempFile.open(QIODevice::ReadWrite) && correspondenceFile.open(QIODevice::ReadWrite)) {
            persistCorrespondenceToFile(correspondenceFile, tempFile, objectImageCorrespondence, deleteCorrespondence);
            //! It might be that both files have been deleted when they did not contain any content anymore
            if (tempFile.exists() && correspondenceFile.exists()) {
                correspondenceFile.remove();
                tempFile.copy(correspondenceFilePath.path());
                tempFile.remove();
            }
        } else {
            throw "Could not create temporary file or could not open/read correspondence file";
        }
    } else {
        QFile outFile(correspondenceFilePath.path());
        if (outFile.open(QIODevice::ReadWrite)) {
            QTextStream outStream(&outFile);
            outStream << objectImageCorrespondence.toString();
            outFile.close();
        } else {
            throw "Could not create new correspondence file";
        }
    }

    return true;
}

QList<Image> TextFileLoadAndStoreStrategy::loadImages() {
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
                images.push_back(Image(imageFilename, imagesPath.path(), segmentationImageFilename));
            } else {
                //! We didn't find a segmentation image, i.e. two separate images have to be added
                images.push_back(Image(imageFilename, imagesPath.path()));
                images.push_back(Image(secondImageFilename, imagesPath.path()));
            }
        } else {
            images.push_back(Image(imageFilename, imagesPath.path()));
        }
    }

    return images;
}

QList<ObjectModel> TextFileLoadAndStoreStrategy::loadObjectModels() {
    QList<ObjectModel> objectModels;

    //! See explanation under loadImages for why we don't throw an exception here
    if (!pathExists(objectModelsPath.path())) {
        return objectModels;
    }

    for (QString directory : objectModelsPath.entryList(QDir::Dirs, QDir::Name)) {
        QStringList fileFilter;
        for (const QString& extension : OBJECT_MODEL_FILES_EXTENSIONS) {
            fileFilter << ("*" + extension);
        }
        for (QString file : QDir(objectModelsPath.filePath(directory)).entryList(fileFilter, QDir::Files, QDir::Name)) {
            //! We only store the path to the model file that is relative to the object models path
            //! so that we can modify the object models path and still find the object model
            QString relativeFilePath = directory + "/" + file;
            objectModels.push_back(ObjectModel(relativeFilePath, objectModelsPath.path()));
            //! There should only be one .obj (or other extension) file entry, when found continue
            continue;
        }
    }

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

void loadCorrespondencesFromFile(const QString &inFilePath, QList<ObjectImageCorrespondence> &correspondences,
                       QMap<QString, const Image*> &imageMap, QMap<QString, const ObjectModel*> &objectModelMap) {
    QFile inFile(inFilePath);
    if (inFile.open(QIODevice::ReadOnly)) {
        //! Read every line
        QTextStream in(&inFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList splitLine = line.split(TextFileLoadAndStoreStrategy::CORRESPONDENCE_FORMAT_DELIMITER);
            if (splitLine.size() != 10) {
                throw "Correspondences in wrong format.";
            }
            //! Re-construct the correspondence from the line
            const Image *image = imageMap[splitLine[1]];
            const ObjectModel *objectModel = objectModelMap[splitLine[2]];

            if (image && objectModel) {
                //! We only load correspondences if the image and object model is found
                //! Do not throw an exception here, maybe the path is not correctly set
                //! but instead simply don't load correspondences that don't find their
                //! entities
                ObjectImageCorrespondence correspondence(splitLine[0], // ID
                                                      splitLine[3].toInt(), splitLine[4].toInt(), splitLine[5].toInt(), // Pos
                                                      splitLine[6].toInt(), splitLine[7].toInt(), splitLine[8].toInt(), // Rot
                                                      splitLine[9].toFloat(), // Articulation
                                                      image,
                                                      objectModel);
                correspondences.push_back(correspondence);
            }
        }
    }
}

QList<ObjectImageCorrespondence> TextFileLoadAndStoreStrategy::loadCorrespondences(const QList<Image> &images, const QList<ObjectModel> &objectModels) {
    QList<ObjectImageCorrespondence> correspondences;

    //! See loadImages for why we don't throw an exception here
    if (!pathExists(correspondencesPath)) {
        return correspondences;
    }

    QMap<QString, const Image*> imageMap = createImageMap(images);
    QMap<QString,const ObjectModel*> objectModelMap = createObjectModelMap(objectModels);

    QStringList fileFilter("*" + TextFileLoadAndStoreStrategy::CORRESPONDENCE_FILES_EXTENSION);
    QDirIterator it(correspondencesPath.path(), fileFilter, QDir::Files);

    while (it.hasNext()) {
        QString file = it.next();
        loadCorrespondencesFromFile(file, correspondences, imageMap, objectModelMap);
    }

    return correspondences;
}



bool TextFileLoadAndStoreStrategy::setImagesPath(const QDir &path) {
    if (!pathExists(path))
        throw "Images path does not exist";
    if (imagesPath == path)
        return true;

    imagesPath = path;

    emit imagesPathChanged();

    return true;
}

QDir TextFileLoadAndStoreStrategy::getImagesPath() const {
    return  imagesPath.path();
}

bool TextFileLoadAndStoreStrategy::setObjectModelsPath(const QDir &path) {
    if (!pathExists(path))
        throw "Object models path does not exist";
    if (objectModelsPath == path)
        return true;

    objectModelsPath = path;

    emit objectModelsPathChanged();

    return true;
}

QDir TextFileLoadAndStoreStrategy::getObjectModelsPath() const {
    return objectModelsPath;
}

bool TextFileLoadAndStoreStrategy::setCorrespondencesPath(const QDir &path) {
    if (!pathExists(path))
        throw "Correspondences path does not exist";
    if (correspondencesPath == path)
        return true;

    correspondencesPath = path;

    emit correspondencesPathChanged();

    return true;
}

QDir TextFileLoadAndStoreStrategy::getCorrespondencesPath() const {
    return correspondencesPath.path();
}

void TextFileLoadAndStoreStrategy::setSegmentationImageFilesSuffix(const QString &suffix) {
    //! Only set suffix if it differs from the suffix before because we then have to reload images
    if (suffix.compare("") != 0 && segmentationImageFilesSuffix.compare("") != 0) {
        segmentationImageFilesSuffix = suffix;
        emit imagesChanged();
    }
}

QString TextFileLoadAndStoreStrategy::getSegmentationImageFilesSuffix() {
    return segmentationImageFilesSuffix;
}

void TextFileLoadAndStoreStrategy::setImageFilesExtension(const QString &extension) {
    if (extension.compare("") && imageFilesExtension.compare(extension) != 0) {
        imageFilesExtension = extension;
        emit imagesChanged();
    }
}

QString TextFileLoadAndStoreStrategy::getImageFilesExtension() {
    return imageFilesExtension;
}
