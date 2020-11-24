#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QVector2D>
#include <QImage>

BackgroundImageRenderable::BackgroundImageRenderable(Qt3DCore::QNode *parent,
                                                     const QString &image)
    : Qt3DCore::QEntity(parent) {
    mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setWidth(2);
    mesh->setHeight(2);
    material = new Qt3DExtras::QTextureMaterial();
    texture = new Qt3DRender::QTexture2D();
    textureImage = new Qt3DRender::QTextureImage();
    textureImage->setSource(QUrl::fromLocalFile(image));
    textureImage->setMirrored(false);
    texture->addTextureImage(textureImage);
    material->setTexture(texture);
    transform = new Qt3DCore::QTransform();
    transform->setRotationX(90);
    objectPicker = new Qt3DRender::QObjectPicker();
    connect(objectPicker, &Qt3DRender::QObjectPicker::clicked,
            this, &BackgroundImageRenderable::clicked);
    connect(objectPicker, &Qt3DRender::QObjectPicker::moved,
            this, &BackgroundImageRenderable::moved);
    connect(objectPicker, &Qt3DRender::QObjectPicker::pressed,
            this, &BackgroundImageRenderable::pressed);
    objectPicker->setDragEnabled(true);
    objectPicker->setHoverEnabled(true);
    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform);
    // This causes the object pickers of the poses to fire two signals when the mouse
    // is moved while being pressed: one for the pose and one for the background image.
    // This seems to be a bug in Qt3D and we disable the picking of the background image
    // until it is solved. Otherwise we would have to check the depth of the picking event
    // in the picking slots which might cause issues depending on the poses to recover
    // (they might have small depth and our check would fail any clicks on the objects).
    // Unfortunatley, this means that the user can't deselect a pose by clicking the
    // background image renderable.
    // ATTENTION: In addition to not adding the object picker, picking is disabled in
    // the framegraph branch of the background image because without this node the
    // errors still remained -> checkout the PoseViewer3DWidget's setup code, there is
    // a QNoPicking node
    //this->addComponent(objectPicker);
}

BackgroundImageRenderable::~BackgroundImageRenderable() {
}

void BackgroundImageRenderable::setImage(const QString &image) {
    textureImage->setSource(QUrl::fromLocalFile(image));
}
