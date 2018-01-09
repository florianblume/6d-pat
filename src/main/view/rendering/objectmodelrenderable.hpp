#ifndef OBJECTMODELRENDERING_H
#define OBJECTMODELRENDERING_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QPhongMaterial>

class ObjectModelRenderable : public Qt3DCore::QEntity
{
public:
    explicit ObjectModelRenderable(Qt3DCore::QNode *parent, const QString &meshPath, const QString &texturePath);
    ~ObjectModelRenderable();
    Qt3DRender::QMesh *getMesh() const;
    Qt3DCore::QTransform *getTransform() const;
    QString getMeshPath() const;

private:
    Qt3DRender::QMesh *mesh;
    Qt3DCore::QTransform *transform;
    Qt3DRender::QTextureImage *textureImage;
    Qt3DRender::QAbstractTexture *texture;
    Qt3DExtras::QPhongMaterial *material;

    QString meshPath;
};

#endif // OBJECTMODELRENDERING_H
