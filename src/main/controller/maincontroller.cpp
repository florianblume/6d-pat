#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSettings>
#include <QDir>
#include <iostream>

// Empty initialization of strategy so that we can set the path later and do so
// in a background thread to keep application reactive
MainController::MainController() :
    strategy(),
    modelManager(strategy) {
    connect(preferencesStore.get(), SIGNAL(preferencesChanged(QString)),
            this, SLOT(onPreferencesChanged(QString)));
    correspondenceCreator.reset(new CorrespondenceCreator(0, &modelManager));
    // Whenever the user clicks the create button in the correspondence editor we need to reset
    // the controller as well
    connect(&modelManager, SIGNAL(correspondenceAdded(QString)),
            this, SLOT(resetCorrespondenceCreation()));
    connect(&modelManager, SIGNAL(correspondenceDeleted(QString)),
            this, SLOT(resetCorrespondenceCreation()));
    connect(&strategy, SIGNAL(failedToLoadImages(QString)), this, SLOT(onFailedToLoadImages(QString)));
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
    strategy.setCorrespondencesFilePath(currentPreferences->getCorrespondencesFilePath());
    strategy.setImageFilesExtension(currentPreferences->getImageFilesExtension());
    strategy.setSegmentationImageFilesSuffix(currentPreferences->getSegmentationImageFilesSuffix());
    initializeMainWindow();
}

void MainController::initializeMainWindow() {
    // Notifies the main window of the initialization process so that it can display a message etc.
    mainWindow.onInitializationStarted();

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

    // Delegation of user clicks to this controller
    connect(&mainWindow, SIGNAL(imageClicked(Image*,QPoint)),
            this, SLOT(onImageClicked(Image*,QPoint)));
    connect(&mainWindow, SIGNAL(objectModelClicked(ObjectModel*,QVector3D)),
            this, SLOT(onObjectModelClicked(ObjectModel*,QVector3D)));

    // Delegation of new paths set in the navigation controls of the UI to this controller
    connect(&mainWindow, SIGNAL(imagesPathChanged(QString)),
            this, SLOT(onImagePathChanged(QString)));
    connect(&mainWindow, SIGNAL(objectModelsPathChanged(QString)),
            this, SLOT(onObjectModelsPathChanged(QString)));

    // ## Correspondence stuff

    // Delegation of correspondence creator actions to the window
    connect(correspondenceCreator.get(), SIGNAL(correspondenceCreationAborted()),
            &mainWindow, SLOT(onCorrespondenceCreationReset()));
    connect(correspondenceCreator.get(), SIGNAL(correspondencePointStarted(QPoint,int,int)),
            &mainWindow, SLOT(onCorrespondencePointStarted(QPoint,int,int)));
    connect(correspondenceCreator.get(), SIGNAL(correspondencePointFinished(QVector3D,int,int)),
            &mainWindow, SLOT(onCorrespondencePointFinished(QVector3D,int,int)));

    // Delegate of user interactions to the controller
    connect(&mainWindow, SIGNAL(correspondenceCreationInterrupted()),
            this, SLOT(onCorrespondenceCreationInterrupted()));
    connect(&mainWindow, SIGNAL(correspondenceCreationAborted()),
            this, SLOT(onCorrespondenceCreationAborted()));
    connect(&mainWindow, SIGNAL(requestCorrespondenceCreation()),
            this, SLOT(onCorrespondenceCreationRequested()));

    connect(&mainWindow, &MainWindow::correspondencePredictionRequested,
            this, &MainController::onCorrespondencePredictionRequested);


    mainWindow.onInitializationCompleted();
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    galleryObjectModelModel->setSegmentationCodesForObjectModels(currentPreferences->getSegmentationCodes());
}

void MainController::onImageClicked(Image* image, QPoint position) {
    if (correspondenceCreator->getState() != CorrespondenceCreator::State::CorrespondencePointStarted) {
        // We can set the image here everytime, if it differs from the previously one, the creator will
        // automatically reset the points etc.
        correspondenceCreator->setImage(image);
        correspondenceCreator->startCorrespondencePoint(position);
    }
}

void MainController::onObjectModelClicked(ObjectModel* objectModel, QVector3D position) {
    if (correspondenceCreator->isImageSet() && correspondenceCreator->getState() ==
                                               CorrespondenceCreator::State::CorrespondencePointStarted) {
        correspondenceCreator->setObjectModel(objectModel);
        correspondenceCreator->finishCorrespondencePoint(position);
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
    mainWindow.raise();
}

void MainController::resetCorrespondenceCreation() {
    correspondenceCreator->abortCreation();
}

void MainController::onCorrespondenceCreationRequested() {
    // The user can't request this before all the requirements are met because the creat button
    // is not enabled earlier
    correspondenceCreator->createCorrespondence();
}

void MainController::onCorrespondencePredictionRequested() {
    if (networkController.isNull()) {
        networkController.reset(
                    new NeuralNetworkController(currentPreferences->getTrainingScriptPath(),
                                                currentPreferences->getInferenceScriptPath()));
        connect(networkController.get(), &NeuralNetworkController::inferenceFinished,
                this, &MainController::onNetworkInferenceFinished);
    } else {
        networkController->setTrainPythonScript(currentPreferences->getTrainingScriptPath());
        networkController->setInferencePythonScript(currentPreferences->getInferenceScriptPath());
    }
    networkController->setImages(QVector<Image>() << *mainWindow.getCurrentlyViewedImage());
    networkController->setCorrespondencesFilePath(strategy.getCorrespondencesFilePath().path());
    networkController->inference(currentPreferences->getNetworkConfigPath());
}

void MainController::onNetworkTrainingFinished() {
    // Nothing to do here, training not yet implemented
}

void MainController::onNetworkInferenceFinished() {
    mainWindow.hideNetworkProgressView();
}

void MainController::onFailedToLoadImages(const QString &message){
    QString imagesPath = strategy.getImagesPath().path();
    if (imagesPath != "." && imagesPath != "" && strategy.getImageFilesExtension() != "") {
        //! "." is the default path and will likely never be used as image path, thus
        //! do not display any warning - otherwise the warning would pop up three times
        //! on startup, because the program tries to load images when setting the
        //! images extension, the paths, etc.
        mainWindow.displayWarning("Error loading images", message);
    }
}

void MainController::onPreferencesChanged(const QString &identifier) {
    currentPreferences = std::move(preferencesStore->loadPreferencesByIdentifier(identifier));
    // Do checks to avoid unnecessary reloads
    if (currentPreferences->getImagesPath().compare(strategy.getImagesPath().path()) != 0)
        strategy.setImagesPath(currentPreferences->getImagesPath());
    if (currentPreferences->getObjectModelsPath().compare(strategy.getObjectModelsPath().path()) != 0)
        strategy.setObjectModelsPath(currentPreferences->getObjectModelsPath());
    if (currentPreferences->getCorrespondencesFilePath().compare(strategy.getCorrespondencesFilePath().path()) != 0)
        strategy.setCorrespondencesFilePath(currentPreferences->getCorrespondencesFilePath());
    if (currentPreferences->getImageFilesExtension().compare(strategy.getImageFilesExtension()) != 0)
        strategy.setImageFilesExtension(currentPreferences->getImageFilesExtension());
    if (currentPreferences->getSegmentationImageFilesSuffix().compare(strategy.getSegmentationImageFilesSuffix()) != 0)
        strategy.setSegmentationImageFilesSuffix(currentPreferences->getSegmentationImageFilesSuffix());
    setSegmentationCodesOnGalleryObjectModelModel();
    correspondenceCreator->abortCreation();
}
