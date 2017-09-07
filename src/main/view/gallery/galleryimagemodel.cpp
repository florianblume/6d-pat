#include "galleryimagemodel.h"
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    this->modelManager = modelManager;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DecorationRole) {
        vector<Image>* images = modelManager->getImages();
        return QIcon(QString((images->at(index.row())).getAbsoluteImagePath().c_str()));
    } else if (role == Qt::ToolTipRole) {
        vector<Image>* images = modelManager->getImages();
        return QString((images->at(index.row())).getImagePath().c_str());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &parent) const {
    if (modelManager) {
        return modelManager->getImages()->size();
    }
    return 0;
}
