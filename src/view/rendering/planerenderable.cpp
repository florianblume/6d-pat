#include "planerenderable.hpp"

PlaneRenderable::PlaneRenderable(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent) {
    mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(2);
    mesh->setHeight(2);
    material = new Qt3DExtras::QTextureMaterial();
    texture = new Qt3DRender::QTexture2D();
    material->setTexture(texture);
    transform = new Qt3DCore::QTransform();
    transform->setRotationX(90);
    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform);
}
