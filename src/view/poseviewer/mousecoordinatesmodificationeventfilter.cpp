#include "mousecoordinatesmodificationeventfilter.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

MouseCoordinatesModificationEventFilter::MouseCoordinatesModificationEventFilter(QObject *parent):
       QObject(parent) {
}

void MouseCoordinatesModificationEventFilter::setOffset(int x, int y) {
    m_offsetX = x;
    m_offsetY = y;
}

void MouseCoordinatesModificationEventFilter::setOffset(QPoint offset) {
    m_offsetX = offset.x();
    m_offsetY = offset.y();
}

QPoint MouseCoordinatesModificationEventFilter::offset() {
    return QPoint(m_offsetX, m_offsetY);
}

bool MouseCoordinatesModificationEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::HoverMove ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPointF offsetPos = mouseEvent->pos() - QPointF(m_offsetX, m_offsetY);
        mouseEvent->setLocalPos(offsetPos);
        return false;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
