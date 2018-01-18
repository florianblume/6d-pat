#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSettings>
#include <QDir>
#include <iostream>

//! empty initialization of strategy so that we can set the path later and do so
//! in a background thread to keep application reactive
MainController::MainController(int &argc, char *argv[]) :
    QApplication(argc, argv),
    strategy(),
    modelManager(strategy) {
    connect(preferencesStore.get(), SIGNAL(preferencesChanged(QString)),
            this, SLOT(onPreferencesChanged(QString)));
    correspondenceCreator.reset(new CorrespondenceCreator(0, &modelManager));
}

MainController::~MainController() {
    preferencesStore->savePreferences(currentPreferences.get());
    delete galleryImageModel;
    delete galleryObjectModelModel;
}

void MainController::initialize() {
    currentPreferences = std::move(preferencesStore->loadPreferencesByIdentifier("default"));
    strategy.setImagesPath(currentPreferences->getImagesPath());
    strategy.setObjectModelsPath(currentPreferences->getObjectModelsPath());
    strategy.setCorrespondencesPath(currentPreferences->getCorrespondencesPath());
    strategy.setImageFilesExtension(currentPreferences->getImageFilesExtension());
    strategy.setSegmentationImageFilesSuffix(currentPreferences->getSegmentationImageFilesSuffix());
    initializeMainWindow();
}

void MainController::initializeMainWindow() {
    mainWindow.setPreferencesStore(preferencesStore.get());

    //! The reason why the breadcrumbs receive an object of the path type of the boost filesystem library
    //! is because the breadcrumb views have to split the path to show it.
    mainWindow.setPathOnLeftBreadcrumbView(strategy.getImagesPath().path());
    mainWindow.setPathOnRightBreadcrumbView(strategy.getObjectModelsPath().path());
    mainWindow.setPathOnLeftNavigationControls(QString(strategy.getImagesPath().path()));
    mainWindow.setPathOnRightNavigationControls(QString(strategy.getObjectModelsPath().path()));

    // The models do not need to notify the gallery of any changes on the data because the list view
    // has its own update loop, i.e. automatically fetches new data
    galleryImageModel = new GalleryImageModel(&modelManager);
    mainWindow.setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(&modelManager);
    setSegmentationCodesOnGalleryObjectModelModel();
    mainWindow.setGalleryObjectModelModel(galleryObjectModelModel);
    mainWindow.setModelManager(&modelManager);

    mainWindow.setStatusBarText("Ready");

    // Connect the main window's reactions to the user clicking on a displayed image or on an object
    // model to delegate any further computation to this controller
    connect(&mainWindow, SIGNAL(imageClicked(Image*,QPoint)),
            this, SLOT(onImageClicked(Image*,QPoint)));
    connect(&mainWindow, SIGNAL(objectModelClickedAt(ObjectModel*,QVector3D)),
            this, SLOT(onObjectModelClickedAt(ObjectModel*,QVector3D)));
    connect(&mainWindow, SIGNAL(correspondenceCreationInterrupted()),
            this, SLOT(onCorrespondenceCreationInterrupted()));
    connect(&mainWindow, SIGNAL(correspondenceCreationAborted()),
            this, SLOT(onCorrespondenceCreationAborted()));

    connect(&mainWindow, SIGNAL(imagesPathChanged(QString)),
            this, SLOT(onImagePathChanged(QString)));
    connect(&mainWindow, SIGNAL(objectModelsPathChanged(QString)),
            this, SLOT(onObjectModelsPathChanged(QString)));
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    galleryObjectModelModel->setSegmentationCodesForObjectModels(currentPreferences->getSegmentationCodes());
}

void MainController::onImageClicked(Image* image, QPoint position) {
    correspondenceCreator->setImage(image);
    lastClickedImagePosition = position;
    correspondenceCreationSate = CorrespondenceCreationState::ImageClicked;

    // TODO: show how many points are missing until an actual correspondence can be created
    mainWindow.setStatusBarText("Please select the corresponding 3D point [" +
                            QString::number(correspondenceCreator->numberOfCorrespondencePoints() + 1)
                            + " of 4].");
}

void MainController::onObjectModelClickedAt(ObjectModel* objectModel, QVector3D position) {
    // Check whether the correspondencePointCompleted flag is set, so that the point is only
    // added when the user clicked the image somewhere previously
    if (correspondenceCreator->isImageSet() &&
            correspondenceCreationSate == CorrespondenceCreationState::ImageClicked) {
        correspondenceCreationSate = CorrespondenceCreationState::ObjectClicked;
        correspondenceCreator->setObjectModel(objectModel);
        correspondenceCreator->addCorrespondencePoint(lastClickedImagePosition, position);
        int points = correspondenceCreator->numberOfCorrespondencePoints();
        if (points == 0) {
            // If the number of points is 0, although we just added a point, the correspondence
            // creation has been completed and the points cleared
            resetCorrespondenceCreation();
        } else {
            mainWindow.setStatusBarText("Please select another correspondence [" +
                                    QString::number(points + 1)
                                    + " of 4].");
        }
    }
}


void MainController::onCorrespondenceCreationInterrupted() {
    // nothing to do here
}

void MainController::onCorrespondenceCreationAborted() {
    resetCorrespondenceCreation();
}

void MainController::onImagePathChanged(const QString &newPath) {
    this->strategy.setImagesPath(newPath);
    this->currentPreferences->setImagesPath(newPath);
    resetCorrespondenceCreation();
}

void MainController::onObjectModelsPathChanged(const QString &newPath) {
    this->strategy.setObjectModelsPath(newPath);
    this->currentPreferences->setObjectModelsPath(newPath);
    resetCorrespondenceCreation();
}

void MainController::showView() {
    mainWindow.show();
}

void MainController::resetCorrespondenceCreation() {
    correspondenceCreator->abortCreation();
    correspondenceCreationSate = CorrespondenceCreationState::Empty;
    mainWindow.setStatusBarText("Ready.");

}

void MainController::onPreferencesChanged(const QString &identifier) {
    currentPreferences = std::move(preferencesStore->loadPreferencesByIdentifier(identifier));
    // Do checks to avoid unnecessary reloads
    if (currentPreferences->getImagesPath().compare(strategy.getImagesPath().path()) != 0)
        strategy.setImagesPath(currentPreferences->getImagesPath());
    if (currentPreferences->getObjectModelsPath().compare(strategy.getObjectModelsPath().path()) != 0)
        strategy.setObjectModelsPath(currentPreferences->getObjectModelsPath());
    if (currentPreferences->getCorrespondencesPath().compare(strategy.getCorrespondencesPath().path()) != 0)
        strategy.setCorrespondencesPath(currentPreferences->getCorrespondencesPath());
    if (currentPreferences->getImageFilesExtension().compare(strategy.getImageFilesExtension()) != 0)
        strategy.setImageFilesExtension(currentPreferences->getImageFilesExtension());
    if (currentPreferences->getSegmentationImageFilesSuffix().compare(strategy.getSegmentationImageFilesSuffix()) != 0)
        strategy.setSegmentationImageFilesSuffix(currentPreferences->getSegmentationImageFilesSuffix());
    setSegmentationCodesOnGalleryObjectModelModel();
    correspondenceCreator->abortCreation();
}
