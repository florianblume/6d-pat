#include "clickvisualizationmaterial.hpp"

#include "view/misc/displayhelper.hpp"

#include <QPen>
#include <QPainter>
#include <QImage>
#include <QPaintDevice>

ClickVisualizationMaterial::ClickVisualizationMaterial(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent) {
}

void ClickVisualizationMaterial::addClick(QPoint click) {
    clicks.append(click);
}

void ClickVisualizationMaterial::removeClick(QPoint click) {
    for (int i = 0; i < clicks.size(); i++) {
        QPoint _click = clicks.at(i);
        if (_click == click) {
            clicks.remove(i);
        }
    }
}

void ClickVisualizationMaterial::removeClicks() {
    clicks.clear();
}
