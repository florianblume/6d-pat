#include "resizeimagesrunnable.hpp"

#include <QUrl>

ResizeImagesRunnable::ResizeImagesRunnable(const QList<Image> images) :
    images(images) {

}

void ResizeImagesRunnable::run() {
    int i = 0;
    for (Image image : images) {
        if (stopProcess) {
            break;
        }
        QImage loadedImage(QUrl::fromLocalFile(image.getAbsoluteImagePath()).path());
        // No one is going to view images larger than 300 px height
        loadedImage = loadedImage.scaledToHeight(300);
        emit imageResized(i, image.getImagePath(), loadedImage);
        i++;
    }
}

void ResizeImagesRunnable::stop() {
    stopProcess = true;
}
