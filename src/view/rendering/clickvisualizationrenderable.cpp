#include "clickvisualizationrenderable.hpp"

ClickVisualizationRenderable::ClickVisualizationRenderable(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent) {
    mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(1);
    mesh->setHeight(1);
    transform = new Qt3DCore::QTransform();
    transform->setRotationX(90);
    material = new ClickVisualizationMaterial;
    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform);
}

void ClickVisualizationRenderable::addClick(QPoint click) {
    material->addClick(click);
}

void ClickVisualizationRenderable::removeClick(QPoint click) {
    material->removeClick(click);
}

void ClickVisualizationRenderable::removeClicks() {
    material->removeClicks();
}

void ClickVisualizationRenderable::setSize(QSize size) {
    mesh->setWidth(size.width());
    mesh->setHeight(size.height());
}
