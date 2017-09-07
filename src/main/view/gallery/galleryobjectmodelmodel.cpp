#include "galleryobjectmodelmodel.h"
#include <QIcon>

GalleryObjectModelModel::GalleryObjectModelModel(ModelManager* modelManager) : modelManager(modelManager) {

}

//! Implementations of QAbstractListModel
QVariant GalleryObjectModelModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DecorationRole) {
        if (currentSelectedImageIndex != -1 && modelManager) {
            vector<ObjectModel>* objectModels = modelManager->getObjectModels();
            return QIcon(objectModels->at(index.row()).getAbsolutePath().c_str());
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
