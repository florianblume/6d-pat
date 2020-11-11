#include "mainapplication.hpp"

#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    MainApplication app(argc, argv);
    return app.exec();
}
