#include "maincontroller.hpp"
#include <QSettings>

string getStoredImagesPath() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    string imagesPath = settings.value("maincontroller/imagesPath", QString("/home")).toString().toStdString();
    return imagesPath;
}

string getStoredObjectModelsPath() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    return settings.value("maincontroller/objectModelsPath", QString("/home")).toString().toStdString();
}

string getStoredCorrespondencesPath() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    return settings.value("maincontroller/correspondencesPath", QString("/home")).toString().toStdString();
}

MainController::MainController(int &argc, char *argv[]) : QApplication(argc, argv),
    strategy(getStoredImagesPath(), getStoredObjectModelsPath(), getStoredCorrespondencesPath()), modelManager(strategy)
{

}

MainController::~MainController() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("maincontroller");
    settings.setValue("imagesPath", strategy.getImagesPath().c_str());
    settings.setValue("objectModelsPath", strategy.getObjectModelsPath().c_str());
    settings.setValue("correspondencesPath", strategy.getCorrespondencesPath().c_str());
    settings.endGroup();
}

void MainController::showView() {
    mainWindow.show();
    mainWindow.setPathOnLeftBreadcrumbView(strategy.getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(strategy.getObjectModelsPath());
    //! left navigation selects the images folder
    mainWindow.addListenerToLeftNavigationControls([this] (boost::filesystem::path newPath) {this->strategy.setImagesPath(newPath);});
    //! right navigation selects the object models folder
    mainWindow.addListenerToRightNavigationControls([this] (boost::filesystem::path newPath) {this->strategy.setObjectModelsPath(newPath);});
}
