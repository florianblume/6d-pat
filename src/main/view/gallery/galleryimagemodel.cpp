#include "galleryimagemodel.hpp"
#include <QDebug>
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
    imagesCache = modelManager->getImages();
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    // Just in case for some weird asynchronous behavior
    // Not entirely thread-safe but might catch some errors
    if (index.row() >= imagesCache.size())
        return QVariant();

    if (role == Qt::DecorationRole) {
        return QIcon(imagesCache[index.row()].getAbsoluteImagePath());
    } else if (role == Qt::ToolTipRole) {
        return QString(imagesCache[index.row()].getImagePath());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &parent) const {
    return imagesCache.size();
}

void GalleryImageModel::onImagesChanged() {
    imagesCache = modelManager->getImages();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(imagesCache.size() - 1, 0);
    emit dataChanged(top, bottom);
}
