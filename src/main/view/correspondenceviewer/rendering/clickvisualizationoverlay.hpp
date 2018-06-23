#ifndef CLICKVISUALIZATIONOVERLAY_H
#define CLICKVISUALIZATIONOVERLAY_H

#include <QFrame>
#include <QColor>
#include <QPoint>

class ClickVisualizationOverlay : public QFrame {
    Q_OBJECT

public:
    ClickVisualizationOverlay(QWidget *parent = 0);

    void addClickedPoint(QPoint click, QColor color);

    void removeClickedPoints();

protected:
    void paintEvent(QPaintEvent *event);

private:
    struct Click {
        QPoint position;
        QColor color;
    };

    QVector<Click> clickedPoints;
};

#endif // CLICKVISUALIZATIONOVERLAY_H
