#include "objectmodelrenderable.hpp"
#include <Qt3DExtras/QPhongMaterial>

ObjectModelRenderable::ObjectModelRenderable(QNode *parent, const QString &meshPath, const QString &texturePath)
  : Qt3DCore::QEntity(parent),
    mesh(new Qt3DRender::QMesh()),
    transform(new Qt3DCore::QTransform()),
    material(new Qt3DExtras::QDiffuseMapMaterial()),
    textureImage(new Qt3DRender::QTextureImage()),
    texture(material->diffuse()),
    meshPath(meshPath)
{
    addComponent(mesh);
    addComponent(transform);
    mesh->setSource(QUrl::fromLocalFile(meshPath));

    // TODO: Temporary before loading the actual material works
    phongMaterial = new Qt3DExtras::QPhongMaterial(this);
    phongMaterial->setDiffuse(QColor(QRgb(0xbeb32b)));
    addComponent(phongMaterial);

    //texture->addTextureImage(textureImage);
    //textureImage->setSource(QUrl(texturePath));
    //addComponent(material);
}


ObjectModelRenderable::~ObjectModelRenderable() {
    delete mesh;
    delete transform;
    delete material;
    delete textureImage;
    delete phongMaterial;
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
