#include "controller/maincontroller.hpp"
#include "misc/global.h"
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");
    MainController m(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    format.setStencilBufferSize(STENCIL_BUFFER_SIZE);
    format.setSamples(NUMBER_OF_SAMPLES);
    QSurfaceFormat::setDefaultFormat(format);

    //! in this order so that the user sees something already and then load entities
    m.showView();
    m.initialize();
    return m.exec();
}
