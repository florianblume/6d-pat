#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QVector2D>
#include <QImage>

BackgroundImageRenderable::BackgroundImageRenderable(Qt3DCore::QNode *parent,
                                                     const QString &image)
    : Qt3DCore::QEntity(parent) {
    mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(2);
    mesh->setHeight(2);
    material = new Qt3DExtras::QTextureMaterial();
    texture = new Qt3DRender::QTexture2D();
    textureImage = new Qt3DRender::QTextureImage();
    textureImage->setSource(QUrl::fromLocalFile(image));
    textureImage->setMirrored(false);
    texture->addTextureImage(textureImage);
    material->setTexture(texture);
    transform = new Qt3DCore::QTransform();
    transform->setRotationX(90);
    objectPicker = new Qt3DRender::QObjectPicker();
    connect(objectPicker, &Qt3DRender::QObjectPicker::clicked, this, &BackgroundImageRenderable::clicked);
    connect(objectPicker, &Qt3DRender::QObjectPicker::moved, this, &BackgroundImageRenderable::moved);
    objectPicker->setDragEnabled(true);
    objectPicker->setHoverEnabled(true);
    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform);
    this->addComponent(objectPicker);
}

BackgroundImageRenderable::~BackgroundImageRenderable() {
}

void BackgroundImageRenderable::setImage(const QString &image) {
    textureImage->setSource(QUrl::fromLocalFile(image));
}
