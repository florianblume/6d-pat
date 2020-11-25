#include "resizeimagesrunnable.hpp"

#include <QUrl>

ResizeImagesRunnable::ResizeImagesRunnable(const QList<ImagePtr> &images) :
    images(images) {

}

void ResizeImagesRunnable::run() {
    int i = 0;
    for (ImagePtr &image : images) {
        if (stopProcess) {
            break;
        }
        QImage loadedImage(QUrl::fromLocalFile(image->absoluteImagePath()).path());
        // No one is going to view images larger than 300 px height
        loadedImage = loadedImage.scaledToHeight(300);
        emit imageResized(i, image->imagePath(), loadedImage);
        i++;
    }
}

void ResizeImagesRunnable::stop() {
    stopProcess = true;
}
