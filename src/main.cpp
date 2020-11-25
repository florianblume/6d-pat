#include "controller/maincontroller.hpp"

int main(int argc, char *argv[]) {
    // Important! This needs to be set before creating the main application
    // so that the context of Qt3D can be shared with OpenGL. Otherwise
    // the Qt3DWidget won't work
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    MainController app(argc, argv);
    return app.exec();
}
