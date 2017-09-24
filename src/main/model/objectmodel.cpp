#include "objectmodel.hpp"
#include <QDir>

ObjectModel::ObjectModel(const QString& objectModelPath, const QString& basePath)
    : objectModelPath(objectModelPath),
      basePath(basePath) {
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
