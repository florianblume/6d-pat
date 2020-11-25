#ifndef CLICKVISUALIZATIONRENDERABLE_H
#define CLICKVISUALIZATIONRENDERABLE_H

#include "view/rendering/clickvisualizationmaterial.hpp"

#include <QObject>
#include <QList>
#include <QPoint>
#include <QSize>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>

class ClickVisualizationRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    ClickVisualizationRenderable(Qt3DCore::QNode *parent = Q_NULLPTR);
    void setClicks(const QList<QPoint> &clicks);
    void setSize(QSize size);
    Qt3DCore::QTransform *transform();

private:
    Qt3DExtras::QPlaneMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;
    ClickVisualizationMaterial *m_material;
};

#endif // CLICKVISUALIZATIONRENDERABLE_H
