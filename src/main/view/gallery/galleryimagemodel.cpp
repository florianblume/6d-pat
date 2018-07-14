#include "galleryimagemodel.hpp"
#include <QDebug>
#include <QIcon>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
    imagesCache = modelManager->getImages();
    resizeImages();
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    // Just in case for some weird asynchronous behavior
    // Not entirely thread-safe but might catch some errors
    if (index.row() >= imagesCache.size())
        return QVariant();

    if (role == Qt::DecorationRole) {
        return QIcon(QPixmap::fromImage(resizedImagesCache[index.row()]));
    } else if (role == Qt::ToolTipRole) {
        return QString(imagesCache[index.row()].getImagePath());
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &/* parent */) const {
    return imagesCache.size();
}

void GalleryImageModel::resizeImages() {
    resizedImagesCache.clear();
    for (Image image : imagesCache) {
        QImage loadedImage(QUrl::fromLocalFile(image.getAbsoluteImagePath()).path());
        // No one is going to view images larger than 300 px height
        loadedImage = loadedImage.scaledToHeight(300);
        resizedImagesCache.append(loadedImage);
    }
}

void GalleryImageModel::onImagesChanged() {
    imagesCache = modelManager->getImages();
    resizeImages();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(imagesCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}
