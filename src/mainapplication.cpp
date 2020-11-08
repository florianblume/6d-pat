#include "mainapplication.hpp"

#include <QFontDatabase>
#include <QTimer>
#include <QObject>

MainApplication::MainApplication(int &argc, char **argv, int) : QApplication(argc, argv) {

}

int MainApplication::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

    MainController m;
    m.initialize();

    QFontDatabase::addApplicationFont(":/fonts/fontawesome-4.7.0.ttf");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(0);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);

    QTimer::singleShot(1500, &splash, &QWidget::close);
    QTimer::singleShot(1500, &m, &MainController::showView);
    return QApplication::exec();
}
