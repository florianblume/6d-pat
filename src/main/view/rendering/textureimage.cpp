#include "textureimage.h"

TextureImage::TextureImage(Qt3DCore::QNode *parent) : Qt3DRender::QPaintedTextureImage(parent) {

}

void TextureImage::paint(QPainter *painter) {
    painter->drawImage(0, 0, QImage(imagePath));
    painter->end();
}

void TextureImage::setImagePath(const QString &imagePath) {
    this->imagePath = imagePath;
    update();
}

QString TextureImage::getImagePath() {
    return this->imagePath;
}
