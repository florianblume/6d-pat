#include "controller/maincontroller.hpp"
#include <QApplication>
#include <QSurfaceFormat>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>

class MainApplication : public QApplication {
public:
    MainApplication(int &argc, char **argv, int = ApplicationFlags);
    static int exec();
};
