#include "objectmodel.h"

ObjectModel::ObjectModel(string _objectModelUrl) : objectModelUrl(_objectModelUrl) {
}

string ObjectModel::getObjectModelUrl() {
    return objectModelUrl;
}
