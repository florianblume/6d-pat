#ifndef DRAGGABLELABEL_H
#define DRAGGABLELABEL_H

#include <QLabel>
#include <QPoint>
#include <QMouseEvent>

class ImageViewLabel : public QLabel
{

    Q_OBJECT

public:
    ImageViewLabel(QFrame *frame);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void setScalingFactor(float scalingFactor);
    float getScalingFactor();
    void reset();

signals:
    void imageClicked(QPoint position);

private:
    QPoint position;
    bool mouseDown = false;
    bool mouseMoved = false;
    float scalingFactor = 1.f;
};

#endif // DRAGGABLELABEL_H
