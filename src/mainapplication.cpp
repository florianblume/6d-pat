#include "mainapplication.hpp"

#include <QTimer>

MainApplication::MainApplication(int &argc, char **argv, int) : QApplication(argc, argv) {

}

int MainApplication::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

    MainController m;
    m.initialize();

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(0);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);

    QTimer::singleShot(2000, &splash, &QWidget::close);
    QTimer::singleShot(2000, &m, &MainController::showView);
    return QApplication::exec();
}
