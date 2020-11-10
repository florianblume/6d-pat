#include "clickvisualizationrenderable.hpp"

ClickVisualizationRenderable::ClickVisualizationRenderable(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent) {
    mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(1);
    mesh->setHeight(1);
    material = new ClickVisualizationMaterial;
    transform = new Qt3DCore::QTransform();
    transform->setRotationX(90);
    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform);
}

void ClickVisualizationRenderable::setClicks(const QList<QPoint> &clicks) {
    material->setClicks(clicks);
}

void ClickVisualizationRenderable::setSize(QSize size) {
    mesh->setWidth(size.width());
    mesh->setHeight(size.height());
    material->setHeight(size.height());
}
