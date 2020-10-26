#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QVector2D>
#include <QImage>

BackgroundImageRenderable::BackgroundImageRenderable(Qt3DCore::QNode *parent,
                                                     const QString &image)
    : PlaneRenderable(parent) {
    // Texture is flipped so we must flip the y coordinate
    float transform[9] = {1, 0, 0, 0, -1, 0, 0, 0, 1};
    material->setTextureTransform(QMatrix3x3(transform));
    // Because y coordinates are then negative we need to move the coordinates up
    material->setTextureOffset({0, 1});
    textureImage = new Qt3DRender::QTextureImage;
    texture->addTextureImage(textureImage);
    textureImage->setSource(QUrl::fromLocalFile(image));
    textureImage->setMirrored(true);
}

void BackgroundImageRenderable::setImage(const QString &image) {
    textureImage->setSource(QUrl::fromLocalFile(image));
}
