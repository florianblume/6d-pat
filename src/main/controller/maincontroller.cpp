#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.h"
#include <QSettings>
#include <QDir>

//! empty initialization of strategy so that we can set the path later and do so in a background thread to keep application reactive
MainController::MainController(int &argc, char *argv[]) : QApplication(argc, argv), strategy(), modelManager(strategy) {
    connect(&mainWindow, SIGNAL(imageClicked(QPointF)), this, SLOT(onImageClicked(QPointF)));
    connect(&mainWindow, SIGNAL(objectModelClickedAt(int, QVector3D)), this, SLOT(onObjectModelClickedAt(int,QVector3D)));
}

MainController::~MainController() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("maincontroller");
    settings.setValue("imagesPath", strategy.getImagesPath().path());
    settings.setValue("objectModelsPath", strategy.getObjectModelsPath().path());
    settings.setValue("correspondencesPath", strategy.getCorrespondencesPath().path());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup("maincontroller-settings");
    settings.remove("");
    for (uint i = 0; i < modelManager.getObjectModelsSize(); i++) {
        const ObjectModel* model = modelManager.getObjectModel(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        settings.setValue(model->getAbsolutePath(),
                                     currentSettingsItem->getSegmentationCodeForObjectModel(model));
    }
    settings.endGroup();

    delete currentSettingsItem;
    delete galleryImageModel;
    delete galleryObjectModelModel;
}

void MainController::initializeSettingsItem() {
    currentSettingsItem = new SettingsItem("default", &modelManager);
    currentSettingsItem->setImagesPath(strategy.getImagesPath().path());
    currentSettingsItem->setObjectModelsPath(strategy.getObjectModelsPath().path());
    currentSettingsItem->setCorrespondencesPath(strategy.getCorrespondencesPath().path());
    currentSettingsItem->setSegmentationImageFilesSuffix(strategy.getSegmentationImageFilesSuffix());
    currentSettingsItem->setImageFilesExtension(strategy.getImageFilesExtension());

    //! Read persisted object color codes
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("maincontroller-settings");
    for (uint i = 0; i < modelManager.getObjectModelsSize(); i++) {
        const ObjectModel* model = modelManager.getObjectModel(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        QString storedCode = settings.value(objectModelIdentifier, "").toString();
        if (storedCode.compare("") != 0)
            currentSettingsItem->setSegmentationCodeForObjectModel(model, storedCode);
    }
    settings.endGroup();
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
    mainWindow.addListenerToLeftNavigationControls([this] (QString& newPath) {this->strategy.setImagesPath(newPath);
                                                                             this->currentSettingsItem->setImagesPath(newPath);});
    //! right navigation selects the object models folder
    mainWindow.addListenerToRightNavigationControls([this] (QString& newPath) {this->strategy.setObjectModelsPath(newPath);
                                                                              this->currentSettingsItem->setObjectModelsPath(newPath);});

    //! The models do not need to notify the gallery of any changes on the data because the list view
    //! has its own update loop, i.e. automatically fetches new data
    galleryImageModel = new GalleryImageModel(&modelManager);
    mainWindow.setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(&modelManager);
    setSegmentationCodesOnGalleryObjectModelModel();
    mainWindow.setGalleryObjectModelModel(galleryObjectModelModel);
    mainWindow.setModelManager(&modelManager);

    mainWindow.setStatusTip("Ready");
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    QMap<const ObjectModel*, QString> codes;
    currentSettingsItem->getSegmentationCodes(codes);
    galleryObjectModelModel->setSegmentationCodesForObjectModels(codes);
}

void MainController::onImageClicked(QPointF position) {
    //! Delete the last clicked position if it still exists, this may be because the user clicked again on the image
    //! withtout clicking a 3D correspondence next
    if (lastClickedImagePosition)
        delete lastClickedImagePosition;
    lastClickedImagePosition = new QPointF(position);
    // TODO: show how many points are missing until an actual correspondence can be created
    mainWindow.setStatusTip(QString("Please select a corresponding 3D point [" + correspondingPoints.size()) + " of 4].");
}

void MainController::onObjectModelClickedAt(int objectModelIndex, QVector3D position) {
    //! If we can't find a previously clicked position on the image just return, the user has to select a 2D
    //! point on the image first
    if (!lastClickedImagePosition)
        return;

    if (correspondingPoints.size() < 4) {
        correspondingPoints.append(CorrespondingPoints{*lastClickedImagePosition, objectModelIndex, position});
        delete lastClickedImagePosition;
        lastClickedImagePosition = Q_NULLPTR;
        mainWindow.setStatusTip(QString("Please create another correspondence [" + correspondingPoints.size()) + " of 4].");
    } else {
        mainWindow.setStatusTip("Creating correspondence, please wait.");
        // TODO: create correspondence
        mainWindow.setStatusTip("Ready.");
    }

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
    setSegmentationCodesOnGalleryObjectModelModel();

    //! And update the settings item on the main window for the settings dialog, etc.
    mainWindow.setSettingsItem(currentSettingsItem);
}
