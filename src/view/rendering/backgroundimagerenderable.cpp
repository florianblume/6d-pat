#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QVector2D>
#include <QImage>

BackgroundImageRenderable::BackgroundImageRenderable(Qt3DCore::QNode *parent,
                                                     const QString &image)
    : Qt3DCore::QEntity(parent) {
    backgroundImageMesh = new Qt3DExtras::QPlaneMesh();
    backgroundImageMesh->setWidth(2);
    backgroundImageMesh->setHeight(2);
    backgroundImageMaterial = new Qt3DExtras::QTextureMaterial();
    // Texture is flipped so we must flip the y coordinate
    float transform[9] = {1, 0, 0, 0, -1, 0, 0, 0, 1};
    backgroundImageMaterial->setTextureTransform(QMatrix3x3(transform));
    // Because y coordinates are then negative we need to move the coordinates up
    backgroundImageMaterial->setTextureOffset({0, 1});
    backgroundImageTexture = new Qt3DRender::QTexture2D();
    backgroundImageTextureImage = new Qt3DRender::QTextureImage();
    backgroundImageTextureImage->setSource(QUrl::fromLocalFile(image));
    backgroundImageTextureImage->setMirrored(true);
    backgroundImageTexture->addTextureImage(backgroundImageTextureImage);
    backgroundImageMaterial->setTexture(backgroundImageTexture);
    backgroundImageTransform = new Qt3DCore::QTransform();
    backgroundImageTransform->setRotationX(90);
    this->addComponent(backgroundImageMesh);
    this->addComponent(backgroundImageMaterial);
    this->addComponent(backgroundImageTransform);
}

BackgroundImageRenderable::~BackgroundImageRenderable() {
}

void BackgroundImageRenderable::setImage(const QString &image) {
    backgroundImageTextureImage->setSource(QUrl::fromLocalFile(image));
}
