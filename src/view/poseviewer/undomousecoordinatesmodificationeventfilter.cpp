#include "undomousecoordinatesmodificationeventfilter.hpp"

#include <QEvent>
#include <QMouseEvent>

UndoMouseCoordinatesModificationEventFilter::UndoMouseCoordinatesModificationEventFilter(
        QObject *parent,
        MouseCoordinatesModificationEventFilter *coveringEventFilter)
    : QObject(parent)
    , m_coveringEventFiler(coveringEventFilter) {
}

bool UndoMouseCoordinatesModificationEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::HoverMove ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        // Undo the offset of the mouse coordinates modificator
        QPointF offsetPos = mouseEvent->pos() + m_coveringEventFiler->offset();
        mouseEvent->setLocalPos(offsetPos);
        return false;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
