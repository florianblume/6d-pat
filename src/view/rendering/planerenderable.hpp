#ifndef PLANERENDERABLE_H
#define PLANERENDERABLE_H

#include <QString>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>

class PlaneRenderable : public Qt3DCore::QEntity
{
public:
    PlaneRenderable(Qt3DCore::QNode *parent);

protected:
    Qt3DExtras::QPlaneMesh *mesh;
    Qt3DCore::QTransform *transform;
    Qt3DExtras::QTextureMaterial *material;
    Qt3DRender::QTexture2D *texture;
};

#endif // PLANERENDERABLE_H
