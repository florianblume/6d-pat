#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QObject>
#include <QSplashScreen>

class SplashScreen : public QSplashScreen {
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = 0);

public slots:
    void showProgressBar(bool show);
    void setProgress(int value);
    void setMaximum(int maximum);
    void setMinimum(int minimum);

private:
    void drawContents(QPainter *painter) override;
    void makeValueWithinBounds(int &value);

private:
    bool m_showProgressBar = false;
    int m_progress = 0;
    int m_maximum = 100;
    int m_minimum = 0;
};
#endif // SPLASHSCREEN_H
