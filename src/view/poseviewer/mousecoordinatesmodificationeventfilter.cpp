#include "mousecoordinatesmodificationeventfilter.hpp"

#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

MouseCoordinatesModificationEventFilter::MouseCoordinatesModificationEventFilter(QObject *parent) : QObject(parent) {

}

void MouseCoordinatesModificationEventFilter::setOffset(int x, int y) {
    m_offsetX = x;
    m_offsetY = y;
}

bool MouseCoordinatesModificationEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        qDebug() << mouseEvent->pos();
        QMouseEvent *newEvent = new QMouseEvent(mouseEvent->type(), mouseEvent->pos() - QPoint(m_offsetX, m_offsetY),
                                                mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
        return QObject::eventFilter(obj, newEvent);
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
