#include "galleryobjectmodelmodel.h"

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager) : modelManager(modelManager) {
}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if (currentSelectedImageIndex != -1 && modelManager) {
            vector<ObjectModel>* objectModels = modelManager->getObjectModels();
            return QString(objectModels->at(index.row()).getPath().c_str());
        }
    }
    return QVariant();
}

int GalleryObjectModelModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        int size = modelManager->getObjectModels()->size();
        return size;
    }
    return 0;
}

void GalleryObjectModelModel::onSelectedImageChanged(int index) {
    if (index != currentSelectedImageIndex) {
        currentSelectedImageIndex = index;
        emit displayedObjectModelsChanged();
    }
}
