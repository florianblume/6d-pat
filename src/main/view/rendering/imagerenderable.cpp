#include "imagerenderable.hpp"
#include <QImage>

ImageRenderable::ImageRenderable(Qt3DCore::QNode *parent, const QString &imagePath) :
    Qt3DCore::QEntity(parent),
    mesh(new Qt3DExtras::QPlaneMesh()),
    transform(new Qt3DCore::QTransform()),
    textureMaterial(new Qt3DExtras::QTextureMaterial()),
    texture(new Qt3DRender::QTexture2D()),
    textureImage(new Qt3DRender::QTextureImage())
{
    textureImage->setSource(QUrl::fromLocalFile(imagePath));
    textureImage->setMirrored(false);
    texture->addTextureImage(textureImage);
    textureMaterial->setTexture(texture);
    addComponent(textureMaterial);
    QImage image(imagePath);
    mesh->setWidth(1 * (image.size().width() / (float) image.size().height()));
    mesh->setHeight(1);
    addComponent(mesh);
    transform->setRotationX(90.f);
    addComponent(transform);
}

ImageRenderable::~ImageRenderable() {
    delete mesh;
    delete transform;
    delete textureMaterial;
    //! We do not have to delete the rest, apparently the 3D window takes care of that
}

Qt3DCore::QTransform *ImageRenderable::getTransform() const {
    return transform;
}

QString ImageRenderable::getImagePath() {
    return imagePath;
}
