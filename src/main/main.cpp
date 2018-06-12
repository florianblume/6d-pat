#include "controller/maincontroller.hpp"
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");
    MainController m(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    //! in this order so that the user sees something already and then load entities
    m.showView();
    m.initialize();
    return m.exec();
}
