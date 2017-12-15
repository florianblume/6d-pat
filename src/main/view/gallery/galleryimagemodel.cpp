#include "galleryimagemodel.hpp"
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    this->modelManager = modelManager;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    QList<Image> images = std::move(modelManager->getImages());
    if (role == Qt::DecorationRole) {
        return QIcon(images[index.row()].getAbsoluteImagePath());
    } else if (role == Qt::ToolTipRole) {
        return QString(images[index.row()].getImagePath());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getImages().size();
    }
    return 0;
}
