#include "mainapplication.hpp"

MainApplication::MainApplication(int &argc, char **argv, int) : QApplication(argc, argv) {

}

int MainApplication::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    //splash.setWindowFlag(Qt::WindowStaysOnTopHint, true);
    splash.show();
    splash.raise();
    processEvents();

    MainController m;

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(0);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);
    QThread::sleep(1);

    //! in this order so that the user sees something already and then load entities
    m.initialize();
    m.showView();
    splash.close();
    return QApplication::exec();
}
