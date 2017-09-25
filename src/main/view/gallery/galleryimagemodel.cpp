#include "galleryimagemodel.h"
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    this->modelManager = modelManager;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DecorationRole) {
        QList<Image*> images;
        modelManager->getImages(images);
        return QIcon(images.at(index.row())->getAbsoluteImagePath());
    } else if (role == Qt::ToolTipRole) {
        QList<Image*> images;
        modelManager->getImages(images);
        return QString((images.at(index.row()))->getImagePath());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getImagesSize();
    }
    return 0;
}
