#ifndef UNDOMOUSECOORDINATESMODIFICATIONEVENTFILTER_H
#define UNDOMOUSECOORDINATESMODIFICATIONEVENTFILTER_H

#include "mousecoordinatesmodificationeventfilter.hpp"

#include <QObject>

class UndoMouseCoordinatesModificationEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit UndoMouseCoordinatesModificationEventFilter(
            QObject *parent,
            MouseCoordinatesModificationEventFilter *coveringEventFilter);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    MouseCoordinatesModificationEventFilter *m_coveringEventFiler;
};

#endif // UNDOMOUSECOORDINATESMODIFICATIONEVENTFILTER_H
