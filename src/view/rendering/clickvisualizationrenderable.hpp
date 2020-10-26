#ifndef CLICKVISUALIZATIONRENDERABLE_H
#define CLICKVISUALIZATIONRENDERABLE_H

#include "view/rendering/clickvisualizationmaterial.hpp"

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QSize>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongMaterial>

class ClickVisualizationRenderable : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    ClickVisualizationRenderable(Qt3DCore::QNode *parent = Q_NULLPTR);
    void addClick(QPoint click);
    void removeClicks();
    void setSize(QSize size);

private:
    Qt3DExtras::QPlaneMesh *mesh;
    Qt3DCore::QTransform *transform;
    //ClickVisualizationMaterial *material;
    Qt3DExtras::QPhongMaterial *material;
};

#endif // CLICKVISUALIZATIONRENDERABLE_H
