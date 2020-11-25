#ifndef CLICKVISUALIZATIONOVERLAY_H
#define CLICKVISUALIZATIONOVERLAY_H

#include <QWidget>
#include <QFrame>
#include <QColor>
#include <QPoint>

class MoveableContainerWidget : public QWidget {

    Q_OBJECT

public:
    MoveableContainerWidget(QWidget *parent = 0);
    void setContainedChild(QWidget *containedChild);

public Q_SLOTS:
    void onMouseMoveEvent(QMouseEvent *event);
};

#endif // CLICKVISUALIZATIONOVERLAY_H
