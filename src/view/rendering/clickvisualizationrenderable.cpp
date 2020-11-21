#include "clickvisualizationrenderable.hpp"

ClickVisualizationRenderable::ClickVisualizationRenderable(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent) {
    m_mesh = new Qt3DExtras::QPlaneMesh();
    m_mesh->setWidth(1);
    m_mesh->setHeight(1);
    m_material = new ClickVisualizationMaterial;
    m_transform = new Qt3DCore::QTransform();
    m_transform->setRotationX(90);
    this->addComponent(m_mesh);
    this->addComponent(m_material);
    this->addComponent(m_transform);
}

void ClickVisualizationRenderable::setClicks(const QList<QPoint> &clicks) {
    m_material->setClicks(clicks);
}

void ClickVisualizationRenderable::setSize(QSize size) {
    m_mesh->setWidth(size.width());
    m_mesh->setHeight(size.height());
    m_material->setSize(size);
}

Qt3DCore::QTransform *ClickVisualizationRenderable::transform() {
    return m_transform;
}
