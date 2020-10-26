#ifndef BACKGROUNDIMAGERENDERABLE_H
#define BACKGROUNDIMAGERENDERABLE_H

#include "model/image.hpp"

#include <QString>
#include <QScopedPointer>
#include <QVector>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTextureImage>

class BackgroundImageRenderable : public Qt3DCore::QEntity
{
public:
    BackgroundImageRenderable(Qt3DCore::QNode *parent,
                              const QString &image);
    ~BackgroundImageRenderable();
    void setImage(const QString &image);

private:
    Qt3DExtras::QPlaneMesh *backgroundImageMesh;
    Qt3DCore::QTransform *backgroundImageTransform;
    Qt3DExtras::QTextureMaterial *backgroundImageMaterial;
    Qt3DRender::QTexture2D *backgroundImageTexture;
    Qt3DRender::QTextureImage *backgroundImageTextureImage;
};

#endif // BACKGROUNDIMAGERENDERABLE_H
