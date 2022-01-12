#include "resizeimagesrunnable.hpp"

#include <QUrl>
#include <QImageReader>
#include <QElapsedTimer>

ResizeImagesRunnable::ResizeImagesRunnable(const ImageList &images) {
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
        QElapsedTimer timer;
        timer.start();
        QImageReader imageReader(QUrl::fromLocalFile(image.absoluteImagePath()).path());
        float aspectRatio = imageReader.size().width() / (float) imageReader.size().height();
        // No one is going to view images larger than 300 px height
        imageReader.setScaledSize(QSize(300 * aspectRatio, 300));
        Q_EMIT imageResized(i, image.imagePath(), imageReader.read());
        i++;
    }
}

void ResizeImagesRunnable::stop() {
    m_stopProcess = true;
}
