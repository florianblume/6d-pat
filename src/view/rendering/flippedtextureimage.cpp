#include "flippedtextureimage.h"

#include <QPainter>
#include <QImage>
#include <QPaintDevice>

FlippedTextureImage::FlippedTextureImage(Qt3DCore::QNode *parent)
    : Qt3DRender::QPaintedTextureImage(parent) {
}

void FlippedTextureImage::paint(QPainter *painter) {
    if (!image.isNull()) {
        painter->drawImage(0, 0, *image.data());
    }
}

void FlippedTextureImage::setSource(const QUrl &image) {
    QImage loadedImage(image.path());
    this->setSize(loadedImage.size());
    this->image.reset(new QImage(loadedImage));
}
