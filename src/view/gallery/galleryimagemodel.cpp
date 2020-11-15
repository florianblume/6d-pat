#include "galleryimagemodel.hpp"
#include <QDebug>
#include <QIcon>
#include <QPainter>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
    imagesCache = modelManager->images();
    resizeImages();
    connect(modelManager, &ModelManager::dataChanged,
            this, &GalleryImageModel::onDataChanged);
}

GalleryImageModel::~GalleryImageModel() {
    resizeImagesRunnable->stop();
    resizeImagesThreadpool.waitForDone();
    delete resizeImagesRunnable;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    // Just in case for some weird asynchronous behavior
    // Not entirely thread-safe but might catch some errors
    if (index.row() >= imagesCache.size())
        return QVariant();

    QString imagePath = imagesCache[index.row()]->imagePath();
    if (role == Qt::DecorationRole) {
        if (resizedImagesCache.contains(imagePath)) {
            return QIcon(QPixmap::fromImage(resizedImagesCache[imagePath]));
        } else {
            return QIcon(currentLoadingAnimationFrame);
        }
    } else if (role == Qt::ToolTipRole) {
        return imagePath;
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &/* parent */) const {
    return imagesCache.size();
}

void GalleryImageModel::resizeImages() {
    if (!resizeImagesRunnable.isNull()) {
        resizeImagesRunnable->stop();
        resizeImagesThreadpool.clear();
        resizeImagesThreadpool.waitForDone();
    }
    resizedImagesCache.clear();
    resizeImagesRunnable = new ResizeImagesRunnable(imagesCache);
    connect(resizeImagesRunnable, &ResizeImagesRunnable::imageResized,
            this, &GalleryImageModel::onImageResized);
    resizeImagesThreadpool.start(resizeImagesRunnable);
}

void GalleryImageModel::onImageResized(int imageIndex, const QString &imagePath, const QImage &resizedImage) {
    resizedImagesCache[imagePath] = resizedImage;
    if (imageIndex == imagesCache.size()) {
        updateTimer.stop();
    }
}

void GalleryImageModel::onDataChanged(int data) {
    // Check if images were changed
    if (data & Data::Images) {
        updateTimer.start();
        imagesCache = modelManager->images();
        resizeImages();
        QModelIndex top = index(0, 0);
        QModelIndex bottom = index(imagesCache.size() - 1, 0);
        Q_EMIT dataChanged(top, bottom);
    }
}
