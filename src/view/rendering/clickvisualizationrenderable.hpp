#ifndef CLICKVISUALIZATIONRENDERABLE_H
#define CLICKVISUALIZATIONRENDERABLE_H

#include "view/rendering/planerenderable.hpp"
#include "view/rendering/clickvisualizationtextureimage.hpp"

#include <QSize>

class ClickVisualizationRenderable : public PlaneRenderable
{
    Q_OBJECT
public:
    ClickVisualizationRenderable(Qt3DCore::QNode *parent = Q_NULLPTR);


public Q_SLOTS:
    void addClick(QPoint click);
    void removeClick(QPoint click);
    void removeClicks();
    void setSize(QSize size);

private:
    ClickVisualizationTextureImage *textureImage;
};

#endif // CLICKVISUALIZATIONRENDERABLE_H
