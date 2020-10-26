#ifndef CLICKVISUALIZATIONTEXTURE_H
#define CLICKVISUALIZATIONTEXTURE_H

#include <QVector>
#include <QPoint>
#include <QSize>
#include <Qt3DRender/QPaintedTextureImage>

class ClickVisualizationTextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    ClickVisualizationTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR, QSize size = {100, 100});
    void paint(QPainter *painter) override;

public Q_SLOTS:
    void addClick(QPoint click);
    void removeClick(QPoint click);
    void removeClicks();
    void setSize(QSize size);

private:
    QVector<QPoint> clicks;
};

#endif // CLICKVISUALIZATIONTEXTURE_H
