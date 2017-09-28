#include "galleryimagemodel.h"
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    this->modelManager = modelManager;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DecorationRole) {
        return QIcon(modelManager->getImage(index.row())->getAbsoluteImagePath());
    } else if (role == Qt::ToolTipRole) {
        return QString(modelManager->getImage(index.row())->getImagePath());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getImagesSize();
    }
    return 0;
}
