#include "tutorialscreen.hpp"

#include <QPainter>

TutorialScreen::TutorialScreen(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
}

void TutorialScreen::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(this->geometry(), QColor(100, 100, 100, 100));
}

QRect TutorialScreen::currentViewWindow() {
    return m_currentViewWindow;
}

void TutorialScreen::setCurrentViewWindow(const QRect &currentViewWindow) {
    m_currentViewWindow = currentViewWindow;
    update();
    Q_EMIT currentViewWindowChanged(m_currentViewWindow);
}
