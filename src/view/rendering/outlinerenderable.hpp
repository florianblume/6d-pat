#ifndef OUTLINERENDERABLE_H
#define OUTLINERENDERABLE_H

#include "view/rendering/outlinematerial.hpp"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QTexture>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>
#include <QVector4D>

class OutlineRenderable : public Qt3DCore::QEntity {

public:
    OutlineRenderable(Qt3DCore::QNode *parent = Q_NULLPTR);
    void setOutlineRenderingTexture(Qt3DRender::QTexture2D *outlineTexture);
    void setOutlineColor(const QVector4D &color);
    void setSamples(int samples);
    void setImageSize(const QSize &size);

private:
    Qt3DExtras::QPlaneMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;
    OutlineMaterial *m_material;
};

#endif // OUTLINERENDERABLE_H
