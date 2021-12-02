#include "loadandstorestrategy.hpp"
#include "misc/generalhelper.hpp"
#include "misc/global.hpp"

#include <QCollator>
#include <QDirIterator>

// Overwriteable by subclasses
const QStringList LoadAndStoreStrategy::OBJECT_MODEL_FILES_EXTENSIONS =
                                            QStringList({"*.obj", "*.ply", "*.3ds", "*.fbx"});
const QStringList LoadAndStoreStrategy::IMAGE_FILES_EXTENSIONS =
                                            QStringList({"*.jpg", "*.jpeg", "*.png", "*.tiff"});

LoadAndStoreStrategy::LoadAndStoreStrategy() {
    connectWatcherSignals();
}

LoadAndStoreStrategy::~LoadAndStoreStrategy() {

}

void LoadAndStoreStrategy::applySettings(SettingsPtr settings) {
    setImagesPath(settings->imagesPath());
    setObjectModelsPath(settings->objectModelsPath());
    setPosesFilePath(settings->posesFilePath());
    setSegmentationImagesPath(settings->segmentationImagesPath());
}

void LoadAndStoreStrategy::setImagesPath(const QString &imagesPath) {
    setPath(imagesPath, this->m_imagesPath);
}

void LoadAndStoreStrategy::setSegmentationImagesPath(const QString &path) {
    setPath(path, this->m_segmentationImagesPath);
}

QList<QString> LoadAndStoreStrategy::imagesWithInvalidData() const {
    return m_imagesWithInvalidData;
}

void LoadAndStoreStrategy::setObjectModelsPath(const QString &objectModelsPath) {
    setPath(objectModelsPath, this->m_objectModelsPath);
}

void LoadAndStoreStrategy::setPosesFilePath(const QString &posesFilePath) {
    setPath(posesFilePath, this->m_posesFilePath);
}

QList<QString> LoadAndStoreStrategy::posesWithInvalidData() const {
    return m_posesWithInvalidData;
}

bool LoadAndStoreStrategy::setPath(const QString &path, QString &oldPath) {
    // Only check if path exists if the new path is not equal to NO_PATH
    // NO_PATH is the path set in the beginning when the program is launched the
    // first time
    // We don't do the check here anymore since we do it when loading and otherwise
    // loading might succeed since the path is not changed but the user might
    // assume that it actually was changed and get confused
    //if (!QFileInfo(path).exists() && path != Global::NO_PATH)
        //return false;
    if (oldPath == path)
        return true;

    if (oldPath != "") {
        m_fileSystemWatcher.removePath(oldPath);
    }
    m_fileSystemWatcher.addPath(path);
    oldPath = path;

    return true;
}

void LoadAndStoreStrategy::onDirectoryChanged(const QString &path) {
    if (path == m_imagesPath || path == m_segmentationImagesPath) {
        Q_EMIT dataChanged(Data::Images);
    } else if (path == m_objectModelsPath) {
        Q_EMIT dataChanged(Data::ObjectModels);
    } else if (path == m_posesFilePath) {
        Q_EMIT dataChanged(Data::Poses);
    }
}

void LoadAndStoreStrategy::onFileChanged(const QString &filePath) {
    // Only for images and object models, because storing poses
    // at the pose file path will trigger this signal as well,
    // but we already updated the program accordingly (of course)
    if (filePath == m_posesFilePath) {
        if (!m_ignorePosesFileChanged) {
            Q_EMIT dataChanged(Data::Poses);;
        }
        m_ignorePosesFileChanged = false;
    } else if (filePath.contains(m_imagesPath)
               && IMAGE_FILES_EXTENSIONS.contains(filePath.right(4))) {
        Q_EMIT dataChanged(Data::Images);
    } else if (filePath.contains(m_objectModelsPath)
               && OBJECT_MODEL_FILES_EXTENSIONS.contains(filePath.right(4))) {
        Q_EMIT dataChanged(Data::ObjectModels);
    }
}

void LoadAndStoreStrategy::connectWatcherSignals() {
    connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &LoadAndStoreStrategy::onDirectoryChanged);
    connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &LoadAndStoreStrategy::onFileChanged);
}
