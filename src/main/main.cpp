#include "controller/maincontroller.hpp"
#include "misc/global.h"
#include <QSurfaceFormat>
#include <QSplashScreen>
#include <QPixmap>
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[]) {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");
    QApplication app(argc, argv);

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    splash.setWindowFlag(Qt::WindowStaysOnTopHint, true);
    splash.show();
    splash.raise();

    MainController m;

    QThread::sleep(1);

    QSurfaceFormat format;
    format.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    format.setStencilBufferSize(STENCIL_BUFFER_SIZE);
    format.setSamples(NUMBER_OF_SAMPLES);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);

    //! in this order so that the user sees something already and then load entities
    m.initialize();
    m.showView();
    splash.close();
    return app.exec();
}
