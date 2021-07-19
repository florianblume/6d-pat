#include "backgroundimagerenderable.hpp"

#include <QUrl>
#include <QMatrix3x3>
#include <QVector2D>
#include <QImage>

BackgroundImageRenderable::BackgroundImageRenderable(Qt3DCore::QNode *parent,
                                                     const QString &image)
    : Qt3DCore::QEntity(parent) {
    m_mesh = new Qt3DExtras::QPlaneMesh();
    m_mesh->setWidth(2);
    m_mesh->setHeight(2);
    m_material = new Qt3DExtras::QTextureMaterial();
    m_texture = new Qt3DRender::QTexture2D();
    m_textureImage = new Qt3DRender::QTextureImage();
    m_textureImage->setSource(QUrl::fromLocalFile(image));
    m_textureImage->setMirrored(false);
    m_texture->addTextureImage(m_textureImage);
    m_material->setTexture(m_texture);
    m_transform = new Qt3DCore::QTransform();
    m_transform->setRotationX(90);
    m_objectPicker = new Qt3DRender::QObjectPicker();
    connect(m_objectPicker, &Qt3DRender::QObjectPicker::clicked,
            this, &BackgroundImageRenderable::clicked);
    connect(m_objectPicker, &Qt3DRender::QObjectPicker::moved,
            this, &BackgroundImageRenderable::moved);
    connect(m_objectPicker, &Qt3DRender::QObjectPicker::pressed,
            this, &BackgroundImageRenderable::pressed);
    m_objectPicker->setDragEnabled(true);
    m_objectPicker->setHoverEnabled(true);
    this->addComponent(m_mesh);
    this->addComponent(m_material);
    this->addComponent(m_transform);
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
    m_textureImage->setSource(QUrl::fromLocalFile(image));
}
