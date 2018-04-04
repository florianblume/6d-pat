#include "objectmodelrenderable.hpp"
#include <Qt3DExtras/QPhongMaterial>

ObjectModelRenderable::ObjectModelRenderable(QNode *parent, const QString &meshPath, const QString &texturePath)
  : Qt3DCore::QEntity(parent),
    mesh(new Qt3DRender::QMesh()),
    transform(new Qt3DCore::QTransform()),
    textureImage(new Qt3DRender::QTextureImage()),
    meshPath(meshPath)
{
    addComponent(mesh);
    addComponent(transform);
    mesh->setSource(QUrl::fromLocalFile(meshPath));
}


ObjectModelRenderable::~ObjectModelRenderable() {
    delete mesh;
    delete transform;
    delete textureImage;
}

QString ObjectModelRenderable::getMeshPath() const {
    return meshPath;
}

Qt3DRender::QMesh *ObjectModelRenderable::getMesh() const {
    return mesh;
}

Qt3DCore::QTransform *ObjectModelRenderable::getTransform() const {
    return transform;
}
