#include "resizeimagesrunnable.hpp"

#include <QUrl>

ResizeImagesRunnable::ResizeImagesRunnable(const QList<ImagePtr> &images) {
    for (ImagePtr image: images) {
        m_images.append(*image.get());
    }
}

void ResizeImagesRunnable::run() {
    int i = 0;
    for (Image &image : m_images) {
        if (m_stopProcess) {
            break;
        }
        QImage loadedImage(QUrl::fromLocalFile(image.absoluteImagePath()).path());
        // No one is going to view images larger than 300 px height
        loadedImage = loadedImage.scaledToHeight(300);
        Q_EMIT imageResized(i, image.imagePath(), loadedImage);
        i++;
    }
}

void ResizeImagesRunnable::stop() {
    m_stopProcess = true;
}
