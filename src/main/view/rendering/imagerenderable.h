#ifndef IMAGERENDERABLE_H
#define IMAGERENDERABLE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>

class ImageRenderable : public Qt3DCore::QEntity
{
public:
    ImageRenderable(Qt3DCore::QNode *parent, const QString &imagePath);
    ~ImageRenderable();
    Qt3DCore::QTransform *getTransform() const;
    QString getImagePath();

private:
    Qt3DExtras::QPlaneMesh *mesh;
    Qt3DCore::QTransform *transform;
    Qt3DRender::QTextureImage *textureImage;
    Qt3DRender::QTexture2D *texture;
    Qt3DExtras::QTextureMaterial *textureMaterial;
    QString imagePath;
};

#endif // IMAGERENDERABLE_H
