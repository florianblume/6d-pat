#include "objectmodel.h"

ObjectModel::ObjectModel(string _objectModelUrl) : objectModelUrl(_objectModelUrl) {
}

const string ObjectModel::getPath() const {
    return objectModelUrl;
}
