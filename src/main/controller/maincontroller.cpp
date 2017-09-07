#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.h"
#include <QSettings>
#include <QDir>

//! Constant methods to load the paths.
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

//! empty initialization of strategy so that we can set the path later and do so in a background thread to keep application reactive
MainController::MainController(int &argc, char *argv[]) : QApplication(argc, argv), strategy(), modelManager(strategy)
{
}

MainController::~MainController() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("maincontroller");
    settings.setValue("imagesPath", strategy.getImagesPath().c_str());
    settings.setValue("objectModelsPath", strategy.getObjectModelsPath().c_str());
    settings.setValue("correspondencesPath", strategy.getCorrespondencesPath().c_str());
    settings.endGroup();

    delete currentSettingsItem;
}

void MainController::initializeSettingsItem() {
    currentSettingsItem = new SettingsItem("default", &modelManager);
    currentSettingsItem->setImagesPath(strategy.getImagesPath().c_str());
    currentSettingsItem->setObjectModelsPath(strategy.getObjectModelsPath().c_str());
    currentSettingsItem->setCorrespondencesPath(strategy.getCorrespondencesPath().c_str());
    currentSettingsItem->setSegmentationImageFilesSuffix(strategy.getSegmentationImageFilesSuffix().c_str());
    currentSettingsItem->setImageFilesExtension(strategy.getImageFilesExtension().c_str());
}
void MainController::initializeMainWindow() {
    mainWindow.setSettingsItem(currentSettingsItem);
    mainWindow.setSettingsDialogDelegate(this);

    //! The reason why the breadcrumbs receive an object of the path type of the boost filesystem library
    //! is because the breadcrumb views have to split the path to show it.
    mainWindow.setPathOnLeftBreadcrumbView(strategy.getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(strategy.getObjectModelsPath());
    mainWindow.setPathOnLeftNavigationControls(QString(strategy.getImagesPath().c_str()));
    mainWindow.setPathOnRightNavigationControls(QString(strategy.getObjectModelsPath().c_str()));
    //! left navigation selects the images folder
    mainWindow.addListenerToLeftNavigationControls([this] (boost::filesystem::path newPath) {this->strategy.setImagesPath(newPath);});
    //! right navigation selects the object models folder
    mainWindow.addListenerToRightNavigationControls([this] (boost::filesystem::path newPath) {this->strategy.setObjectModelsPath(newPath);});
    mainWindow.showStatusMessage("Ready");

    //! The models do not need to notify the gallery of any changes on the data because the list view
    //! has its own update loop, i.e. automatically fetches new data
    mainWindow.setGalleryImageModel(new GalleryImageModel(&modelManager));
    mainWindow.setGalleryObjectModelModel(new GalleryObjectModelModel(&modelManager));

    mainWindow.setModelManagerForCorrespondenceEditor(&modelManager);
}

void MainController::initialize() {
    // TODO: initalization in background thread, only disable UI until loading has finished
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("maincontroller");
    strategy.setImagesPath(settings.value("imagesPath", QDir::homePath()).toString().toStdString());
    strategy.setObjectModelsPath(settings.value("objectModelsPath", QDir::homePath()).toString().toStdString());
    strategy.setCorrespondencesPath(settings.value("correspondencesPath", QDir::homePath()).toString().toStdString());
    settings.endGroup();

    initializeSettingsItem();
    initializeMainWindow();
}

void MainController::showView() {
    mainWindow.show();
}

void MainController::applySettings(const SettingsItem* settingsItem) {
    //! Save the settings item to be set
    delete currentSettingsItem;
    this->currentSettingsItem = new SettingsItem(*settingsItem);

    //! Set the values
    strategy.setImagesPath(settingsItem->getImagesPath().toStdString());
    strategy.setObjectModelsPath(settingsItem->getObjectModelsPath().toStdString());
    strategy.setCorrespondencesPath(settingsItem->getCorrespondencesPath().toStdString());
    strategy.setImageFilesExtension(settingsItem->getImageFilesExtension().toStdString());
    strategy.setSegmentationImageFilesSuffix(settingsItem->getSegmentationImageFilesSuffix().toStdString());

    //! Update view accordingly
    mainWindow.setPathOnLeftBreadcrumbView(settingsItem->getImagesPath().toStdString());
    mainWindow.setPathOnLeftNavigationControls(settingsItem->getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(settingsItem->getObjectModelsPath().toStdString());
    mainWindow.setPathOnRightNavigationControls(settingsItem->getObjectModelsPath());
}
