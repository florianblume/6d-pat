#include "clickvisualizationtextureimage.hpp"

#include "view/misc/displayhelper.hpp"

#include <QPen>
#include <QPainter>
#include <QImage>
#include <QPaintDevice>

ClickVisualizationTextureImage::ClickVisualizationTextureImage(Qt3DCore::QNode *parent, QSize size)
    : Qt3DRender::QPaintedTextureImage(parent) {
    Qt3DRender::QPaintedTextureImage::setSize(size);
}

void ClickVisualizationTextureImage::paint(QPainter *painter) {
    int w = painter->device()->width();
    int h = painter->device()->height();

    painter->fillRect(0, 0, w, h, QColor(100, 100, 100));

    QPen linepen(Qt::black);
    linepen.setCapStyle(Qt::RoundCap);
    int size = std::max(this->size().width(), this->size().height());
    linepen.setWidth(size);
    painter->setPen(linepen);
    for (int i = 0; i < clicks.size(); i++) {
        linepen.setColor(DisplayHelper::colorForPosePointIndex(i));
        QPoint click = clicks.at(i);
        painter->drawPoint(click);
    }
}

void ClickVisualizationTextureImage::addClick(QPoint click) {
    clicks.append(click);
}

void ClickVisualizationTextureImage::removeClick(QPoint click) {
    for (int i = 0; i < clicks.size(); i++) {
        QPoint _click = clicks.at(i);
        if (_click == click) {
            clicks.remove(i);
        }
    }
}

void ClickVisualizationTextureImage::removeClicks() {
    clicks.clear();
}

void ClickVisualizationTextureImage::setSize(QSize size) {
    Qt3DRender::QPaintedTextureImage::setSize(size);
}
