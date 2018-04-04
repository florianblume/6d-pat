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

    material = new Qt3DExtras::QPhongMaterial(this);
    material->setAmbient(QColor(100, 100, 100, 255));
    //material->setDiffuse(QColor(200, 200, 200, 255));
    addComponent(material);
}


ObjectModelRenderable::~ObjectModelRenderable() {
    delete mesh;
    delete transform;
    delete textureImage;
    delete material;
    //! We do not have to delete the rest, apparently the 3D window takes care of that
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
