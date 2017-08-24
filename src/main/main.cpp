#include "maincontroller.hpp"

int main(int argc, char *argv[]) {
    MainController m(argc, argv);
    m.showView();
    return m.exec();
}
