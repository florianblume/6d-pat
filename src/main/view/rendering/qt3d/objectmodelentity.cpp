#include "objectmodelentity.hpp"
#include <Qt3DExtras/QPhongMaterial>

ObjectModelEntity::ObjectModelEntity(QNode *parent, const QString &meshPath, const QString &texturePath)
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


ObjectModelEntity::~ObjectModelEntity() {
    delete mesh;
    delete transform;
    delete textureImage;
}

QString ObjectModelEntity::getMeshPath() const {
    return meshPath;
}

Qt3DRender::QMesh *ObjectModelEntity::getMesh() const {
    return mesh;
}

Qt3DCore::QTransform *ObjectModelEntity::getTransform() const {
    return transform;
}
