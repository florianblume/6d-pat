#include "imageviewlabel.h"

ImageViewLabel::ImageViewLabel(QFrame *frame) : QLabel(frame) {
}

void ImageViewLabel::mousePressEvent(QMouseEvent *event) {
    position = event->globalPos() - QPoint(geometry().x(), geometry().y());
    mouseDown = true;
}

void ImageViewLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (!mouseMoved) {
        // The user didn't move the mouse, i.e. only clicked the image
        // Adjust click to scaling
        QPoint position = event->pos();
        position.setX(position.x() * (1 / scalingFactor));
        position.setY(position.y() * (1 / scalingFactor));
        emit imageClicked(position);
    }
    mouseDown = false;
    mouseMoved = false;
}

void ImageViewLabel::mouseMoveEvent(QMouseEvent *event) {
    if (mouseDown) {
        QPoint newPosition = event->globalPos();
        newPosition.setX(newPosition.x() - position.x());
        newPosition.setY(newPosition.y() - position.y());
        move(newPosition);
        mouseMoved = true;
    }
}

void ImageViewLabel::setScalingFactor(float scalingFactor) {
    Q_ASSERT(scalingFactor >= 0.f);
    this->scalingFactor = scalingFactor;
    QSize size = pixmap()->size();
    size.scale(scalingFactor, scalingFactor, Qt::KeepAspectRatio);
    setFixedSize(size);
}

float ImageViewLabel::getScalingFactor() {
    return scalingFactor;
}
