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
    resizeImagesRunnable->stop();
    resizeImagesThreadpool.waitForDone();
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
        } else {
            // Just a placeholder
            QPixmap pix(300, 300);
            pix.fill(Qt::white);
            QPainter painter(&pix);
            painter.setRenderHint(QPainter::TextAntialiasing);
            painter.setFont(QFont("ubuntu", 35));
            painter.drawText(pix.rect(), Qt::AlignVCenter, imagePath);
            return QIcon(pix);
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
    if (resizeImagesRunnable) {
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

void GalleryImageModel::onImageResized(int imageIndex, QString imagePath, QImage resizedImage) {
    resizedImagesCache[imagePath] = resizedImage;
    QModelIndex top = index(imageIndex, 0);
    QModelIndex bottom = index(imageIndex, 0);
    Q_EMIT dataChanged(top, bottom);
}

void GalleryImageModel::onImagesChanged() {
    imagesCache = modelManager->getImages();
    resizeImages();
    QModelIndex top = index(0, 0);
    QModelIndex bottom = index(imagesCache.size() - 1, 0);
    Q_EMIT dataChanged(top, bottom);
}
