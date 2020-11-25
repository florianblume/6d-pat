#include "splashscreen.hpp"

#include <QStyleOptionProgressBar>

SplashScreen::SplashScreen(QWidget *parent) :
    QSplashScreen(parent) {
    this->setPixmap(QPixmap(":/images/splashscreen.png"));
}

void SplashScreen::showProgressBar(bool show) {
    m_showProgressBar = show;
    update();
}

void SplashScreen::setProgress(int value) {
    m_progress = value;
    // Make sure value is within valid bounds
    makeValueWithinBounds(m_progress);
    update();
}

void SplashScreen::setMaximum(int maximum) {
    m_maximum = maximum;
    makeValueWithinBounds(maximum);
    update();
}

void SplashScreen::setMinimum(int minimum) {
    m_minimum = minimum;
    makeValueWithinBounds(m_minimum);
    update();
}

void SplashScreen::drawContents(QPainter *painter) {
  QSplashScreen::drawContents(painter);

  if (m_showProgressBar) {
      QStyleOptionProgressBar pbstyle;
      pbstyle.initFrom(this);
      pbstyle.state = QStyle::State_Enabled;
      pbstyle.textVisible = false;
      pbstyle.minimum = m_minimum;
      pbstyle.maximum = m_maximum;
      pbstyle.progress = m_progress;
      pbstyle.invertedAppearance = false;
      pbstyle.rect = QRect(width() / 2 - 250 / 2, 700, 250, 20);

      style()->drawControl(QStyle::CE_ProgressBar, &pbstyle, painter, this);
  }
}

void SplashScreen::makeValueWithinBounds(int &value) {
    value = qMax(0, value);
    value = qMin(100, value);
}
