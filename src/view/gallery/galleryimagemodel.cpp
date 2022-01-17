#include "galleryimagemodel.hpp"
#include <QDebug>
#include <QIcon>
#include <QPainter>

GalleryImageModel::GalleryImageModel(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->m_modelManager = modelManager;
    m_imagesCache = modelManager->images();
    resizeImages();
    connect(modelManager, &ModelManager::finishedLoadingData,
            this, &GalleryImageModel::onDataChanged);
}

GalleryImageModel::~GalleryImageModel() {
    m_resizeImagesThreadpool.killTimer(0);
    m_resizeImagesRunnable->stop();
    m_resizeImagesThreadpool.waitForDone();
    delete m_resizeImagesRunnable;
}

QVariant GalleryImageModel::data(const QModelIndex &index, int role) const {
    // Just in case for some weird asynchronous behavior
    // Not entirely thread-safe but might catch some errors
    if (index.row() >= m_imagesCache.size())
        return QVariant();

    QString imagePath = m_imagesCache[index.row()].imagePath();
    if (role == Qt::DecorationRole) {
        if (m_resizedImagesCache.contains(imagePath)) {
            return QIcon(QPixmap::fromImage(m_resizedImagesCache[imagePath]));
        } else {
            return QIcon(m_currentLoadingAnimationFrame);
        }
    } else if (role == Qt::ToolTipRole) {
        return imagePath;
    }
    return QVariant();
}

int GalleryImageModel::rowCount(const QModelIndex &/* parent */) const {
    return m_imagesCache.size();
}

void GalleryImageModel::resizeImages() {
    if (!m_resizeImagesRunnable.isNull()) {
        m_resizeImagesRunnable->stop();
        m_resizeImagesThreadpool.clear();
        m_resizeImagesThreadpool.waitForDone();
    }
    m_resizedImagesCache.clear();
    m_resizeImagesRunnable = new ResizeImagesRunnable(m_imagesCache);
    connect(m_resizeImagesRunnable, &ResizeImagesRunnable::imageResized,
            this, &GalleryImageModel::onImageResized);
    m_resizeImagesThreadpool.start(m_resizeImagesRunnable);
}

void GalleryImageModel::onImageResized(int imageIndex, const QString &imagePath, const QImage &resizedImage) {
    m_resizedImagesCache[imagePath] = resizedImage;
    if (imageIndex == m_imagesCache.size()) {
        m_loadingIconUpdateTimer.stop();
    }
}

void GalleryImageModel::onDataChanged(int data) {
    // Check if images were changed
    if (data & Data::Images) {
        m_loadingIconUpdateTimer.start();
        m_imagesCache = m_modelManager->images();
        resizeImages();
        QModelIndex top = index(0, 0);
        QModelIndex bottom = index(m_imagesCache.size() - 1, 0);
        Q_EMIT dataChanged(top, bottom);
    }
}
