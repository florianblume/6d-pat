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
    qRegisterMetaType<LoadAndStoreStrategy::Error>("LoadAndStoreStrategy::Error");
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

QList<ObjectModelPtr> LoadAndStoreStrategy::loadObjectModels() {
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
        Q_EMIT error(ObjectModelsPathDoesNotExist);
        return objectModels;
    } else if (!info.isDir()) {
        Q_EMIT error(ObjectModelsPathIsNotAFolder);
        return objectModels;
    }

    QDirIterator it(m_objectModelsPath, OBJECT_MODEL_FILES_EXTENSIONS, QDir::Files, QDirIterator::Subdirectories);
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
    if (!QFileInfo(path).exists() && path != Global::NO_PATH)
        return false;
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
