#include "objectmodel.hpp"
#include <QDir>

ObjectModel::ObjectModel(const QString& objectModelPath, const QString& basePath)
    : objectModelPath(objectModelPath),
      basePath(basePath) {
}

ObjectModel::ObjectModel(const ObjectModel &other) {
    objectModelPath = other.objectModelPath;
    basePath = other.basePath;
}

QString ObjectModel::getPath() const {
    return objectModelPath;
}

QString ObjectModel::getBasePath() const {
    return basePath;
}

QString ObjectModel::getAbsolutePath() const {
    return QDir(basePath).filePath(objectModelPath);
}

ObjectModel& ObjectModel::operator=(const ObjectModel &other) {
    basePath = other.basePath;
    objectModelPath = other.objectModelPath;
    return *this;
}
