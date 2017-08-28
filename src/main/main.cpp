#include "maincontroller.hpp"

int main(int argc, char *argv[]) {
    MainController m(argc, argv);
    //! in this order so that the user sees something already and then load entities
    m.showView();
    m.initialize();
    return m.exec();
}
