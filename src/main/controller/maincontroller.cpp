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
    settings.setValue("imagesPath", strategy.getImagesPath().path());
    settings.setValue("objectModelsPath", strategy.getObjectModelsPath().path());
    settings.setValue("correspondencesPath", strategy.getCorrespondencesPath().path());
    settings.endGroup();

    delete currentSettingsItem;
}

void MainController::initializeSettingsItem() {
    currentSettingsItem = new SettingsItem("default", &modelManager);
    currentSettingsItem->setImagesPath(strategy.getImagesPath().path());
    currentSettingsItem->setObjectModelsPath(strategy.getObjectModelsPath().path());
    currentSettingsItem->setCorrespondencesPath(strategy.getCorrespondencesPath().path());
    currentSettingsItem->setSegmentationImageFilesSuffix(strategy.getSegmentationImageFilesSuffix());
    currentSettingsItem->setImageFilesExtension(strategy.getImageFilesExtension());
}
void MainController::initializeMainWindow() {
    mainWindow.setSettingsItem(currentSettingsItem);
    mainWindow.setSettingsDialogDelegate(this);

    //! The reason why the breadcrumbs receive an object of the path type of the boost filesystem library
    //! is because the breadcrumb views have to split the path to show it.
    mainWindow.setPathOnLeftBreadcrumbView(strategy.getImagesPath().path());
    mainWindow.setPathOnRightBreadcrumbView(strategy.getObjectModelsPath().path());
    mainWindow.setPathOnLeftNavigationControls(QString(strategy.getImagesPath().path()));
    mainWindow.setPathOnRightNavigationControls(QString(strategy.getObjectModelsPath().path()));
    //! left navigation selects the images folder
    mainWindow.addListenerToLeftNavigationControls([this] (QString& newPath) {this->strategy.setImagesPath(newPath);});
    //! right navigation selects the object models folder
    mainWindow.addListenerToRightNavigationControls([this] (QString& newPath) {this->strategy.setObjectModelsPath(newPath);});
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
    strategy.setImagesPath(settings.value("imagesPath", QDir::homePath()).toString());
    strategy.setObjectModelsPath(settings.value("objectModelsPath", QDir::homePath()).toString());
    strategy.setCorrespondencesPath(settings.value("correspondencesPath", QDir::homePath()).toString());
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
    strategy.setImagesPath(settingsItem->getImagesPath());
    strategy.setObjectModelsPath(settingsItem->getObjectModelsPath());
    strategy.setCorrespondencesPath(settingsItem->getCorrespondencesPath());
    strategy.setImageFilesExtension(settingsItem->getImageFilesExtension());
    strategy.setSegmentationImageFilesSuffix(settingsItem->getSegmentationImageFilesSuffix());

    //! Update view accordingly
    mainWindow.setPathOnLeftBreadcrumbView(settingsItem->getImagesPath());
    mainWindow.setPathOnLeftNavigationControls(settingsItem->getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(settingsItem->getObjectModelsPath());
    mainWindow.setPathOnRightNavigationControls(settingsItem->getObjectModelsPath());
}
