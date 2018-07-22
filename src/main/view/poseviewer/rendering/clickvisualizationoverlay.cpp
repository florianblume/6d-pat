#include "clickvisualizationoverlay.hpp"

#include <QPainter>

ClickVisualizationOverlay::ClickVisualizationOverlay(QWidget *parent) : QFrame(parent) {

}

void ClickVisualizationOverlay::addClickedPoint(QPoint click, QColor color) {
    clickedPoints.append(Click({click, color}));
    update();
}

void ClickVisualizationOverlay::removeClickedPoints() {
    clickedPoints.clear();
    update();
}

void ClickVisualizationOverlay::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    for (Click &click : clickedPoints) {
        QPen pen(click.color);
        pen.setWidth(4);
        painter.setPen(pen);
        painter.drawPoint(click.position);
    }
}
