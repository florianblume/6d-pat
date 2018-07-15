#include "galleryimagemodel.hpp"
#include <QDebug>
#include <QIcon>
#include <QPainter>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
    imagesCache = modelManager->getImages();
    resizeImages();
    connect(modelManager, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
}

GalleryImageModel::~GalleryImageModel() {
    resizedImagesFuture.cancel();
    resizedImagesFuture.waitForFinished();
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    // Just in case for some weird asynchronous behavior
    // Not entirely thread-safe but might catch some errors
    if (index.row() >= imagesCache.size())
        return QVariant();

    QString imagePath = imagesCache[index.row()].getImagePath();
    if (role == Qt::DecorationRole) {
        if (resizedImagesCache.contains(imagePath)) {
            return QIcon(QPixmap::fromImage(resizedImagesCache[imagePath]));
        }
    } else if (role == Qt::ToolTipRole) {
        return imagePath;
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &/* parent */) const {
    return resizedImagesCache.size();
}

void GalleryImageModel::threadedResizeImages() {
    int i = 0;
    for (Image image : imagesCache) {
        beginInsertRows(QModelIndex(), resizedImagesCache.size(), resizedImagesCache.size());
        QImage loadedImage(QUrl::fromLocalFile(image.getAbsoluteImagePath()).path());
        // No one is going to view images larger than 300 px height
        loadedImage = loadedImage.scaledToHeight(300);
        resizedImagesCache[image.getImagePath()] = loadedImage;
        i++;
        endInsertRows();
    }
}

void GalleryImageModel::resizeImages() {
    resizedImagesFuture.cancel();
    resizedImagesCache.clear();
    resizedImagesFuture = QtConcurrent::run(this, &GalleryImageModel::threadedResizeImages);
}

void GalleryImageModel::onImagesChanged() {
    imagesCache = modelManager->getImages();
    resizeImages();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(imagesCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}
