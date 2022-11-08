#include "outlinerenderable.hpp"

OutlineRenderable::OutlineRenderable(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_mesh(new Qt3DExtras::QPlaneMesh)
    , m_transform(new Qt3DCore::QTransform)
    , m_material(new OutlineMaterial) {
    addComponent(m_mesh);
    addComponent(m_transform);
    addComponent(m_material);
    m_mesh->setWidth(2);
    m_mesh->setHeight(2);
    m_transform->setRotationX(90);
}

void OutlineRenderable::setObjectModelRenderingsTextureParameter(Qt3DRender::QTexture2DMultisample *objectModelRenderingsTexture) {
    m_material->setObjectModelRenderingsTextureParameter(objectModelRenderingsTexture);
}

void OutlineRenderable::setSamples(int samples) {
    m_material->setSamples(samples);
}

void OutlineRenderable::setImageSize(const QSize &size) {
    m_material->setImageSize(size);
}
