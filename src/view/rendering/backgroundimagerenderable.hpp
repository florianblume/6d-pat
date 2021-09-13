#ifndef BACKGROUNDIMAGERENDERABLE_H
#define BACKGROUNDIMAGERENDERABLE_H

#include "model/image.hpp"

#include <QString>
#include <QMatrix4x4>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTextureImage>

class BackgroundImageRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    BackgroundImageRenderable(Qt3DCore::QNode *parent,
                              const QString &image);
    ~BackgroundImageRenderable();
    void setImage(const QString &image);

Q_SIGNALS:
    void clicked(Qt3DRender::QPickEvent *pickEvent);
    void moved(Qt3DRender::QPickEvent *pickEvent);
    void pressed(Qt3DRender::QPickEvent *pickEvent);

private:
    Qt3DExtras::QPlaneMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;
    Qt3DExtras::QTextureMaterial *m_material;
    Qt3DRender::QTexture2D *m_texture;
    Qt3DRender::QTextureImage *m_textureImage;
    Qt3DRender::QObjectPicker *m_objectPicker;
};

#endif // BACKGROUNDIMAGERENDERABLE_H
