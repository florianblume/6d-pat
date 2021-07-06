#ifndef MOUSECOORDINATESMODIFICATIONEVENTFILTER_H
#define MOUSECOORDINATESMODIFICATIONEVENTFILTER_H

#include <QObject>

class MouseCoordinatesModificationEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseCoordinatesModificationEventFilter(QObject *parent = nullptr);
    void setOffset(int x, int y);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    int m_offsetX;
    int m_offsetY;

};

#endif // MOUSECOORDINATESMODIFICATIONEVENTFILTER_H
