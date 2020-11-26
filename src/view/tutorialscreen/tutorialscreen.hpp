#ifndef TUTORIALSCREEN_H
#define TUTORIALSCREEN_H

#include <QWidget>
#include <QPaintEvent>
#include <QRect>
#include <QPropertyAnimation>

class TutorialScreen : public QWidget {

    Q_OBJECT

    Q_PROPERTY(QRect currentViewWindow READ currentViewWindow WRITE setCurrentViewWindow NOTIFY currentViewWindowChanged)

public:
    TutorialScreen(QWidget *parent);
    void paintEvent(QPaintEvent *event) override;
    QRect currentViewWindow();
    void setCurrentViewWindow(const QRect &currentViewWindow);

Q_SIGNALS:
    void currentViewWindowChanged(const QRect currentViewWindow);

private:
    QRect m_currentViewWindow;
};

#endif // TUTORIALSCREEN_H
