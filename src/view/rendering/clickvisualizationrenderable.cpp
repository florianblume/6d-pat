#include "clickvisualizationrenderable.hpp"

ClickVisualizationRenderable::ClickVisualizationRenderable(Qt3DCore::QNode *parent)
    : PlaneRenderable(parent) {
    textureImage = new ClickVisualizationTextureImage;
    texture->addTextureImage(textureImage);
}

void ClickVisualizationRenderable::addClick(QPoint click) {
    textureImage->addClick(click);
}

void ClickVisualizationRenderable::removeClick(QPoint click) {
    textureImage->removeClick(click);
}

void ClickVisualizationRenderable::removeClicks() {
    textureImage->removeClicks();
}

void ClickVisualizationRenderable::setSize(QSize size) {
    textureImage->setSize(size);
}
