#include "objectmodel.hpp"

ObjectModel::ObjectModel(path _objectModelPath) : objectModelPath(_objectModelPath) {
}

const path ObjectModel::getPath() const {
    return objectModelPath;
}

const path ObjectModel::getBasePath() const {
    return basePath;
}

const path ObjectModel::getAbsolutePath() const {
    return absolute(objectModelPath, basePath);
}
