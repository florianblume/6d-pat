#ifndef MOUSECOORDINATESMODIFICATIONEVENTFILTER_H
#define MOUSECOORDINATESMODIFICATIONEVENTFILTER_H

#include <QObject>

class MouseCoordinatesModificationEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseCoordinatesModificationEventFilter(QObject *parent = Q_NULLPTR);
    void setOffset(int x, int y);
    void setOffset(QPoint offset);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    int m_offsetX = 0;
    int m_offsetY = 0;
    QObject *m_widgetToProceedWith;

};

#endif // MOUSECOORDINATESMODIFICATIONEVENTFILTER_H
