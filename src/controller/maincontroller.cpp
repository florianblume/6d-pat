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
MainController::MainController() {
    settingsStore.reset(new SettingsStore());
    strategy.reset(new JsonLoadAndStoreStrategy(settingsStore.data(),
                                                settingsIdentifier));
    modelManager.reset(new CachingModelManager(*strategy.data()));
    connect(settingsStore.data(), SIGNAL(settingsChanged(QString)),
            this, SLOT(onSettingsChanged(QString)));
    poseCreator.reset(new PoseRecoverer(0, modelManager.data()));
    connect(strategy.data(), &LoadAndStoreStrategy::failedToLoadImages, this, &MainController::onFailedToLoadImages);
    connect(strategy.data(), &LoadAndStoreStrategy::failedToLoadObjectModels, this, &MainController::onFailedToLoadObjectModels);
    connect(strategy.data(), &LoadAndStoreStrategy::failedToLoadPoses, this, &MainController::onFailedToLoadPoses);
    connect(strategy.data(), &LoadAndStoreStrategy::failedToPersistPose, this, &MainController::onFailedToPersistPose);
}

MainController::~MainController() {
    // Unnecessary, we save preferences anyway when editing them
    //settingsStore->savePreferences(currentSettings.get());
    delete galleryImageModel;
    delete galleryObjectModelModel;
}

void MainController::initialize() {
    currentSettings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    initializeMainWindow();
}

void MainController::initializeMainWindow() {
    // Notifies the main window of the initialization process so that it can display a message etc.
    mainWindow.onInitializationStarted();

    mainWindow.setPreferencesStore(settingsStore.data());

    //! The reason why the breadcrumbs receive an object of the path type of the boost filesystem library
    //! is because the breadcrumb views have to split the path to show it.
    mainWindow.setPathOnLeftBreadcrumbView(currentSettings->getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(currentSettings->getObjectModelsPath());
    mainWindow.setPathOnLeftNavigationControls(QString(currentSettings->getImagesPath()));
    mainWindow.setPathOnRightNavigationControls(QString(currentSettings->getObjectModelsPath()));

    // The models do not need to notify the gallery of any changes on the data because the list view
    // has its own update loop, i.e. automatically fetches new data
    galleryImageModel = new GalleryImageModel(modelManager.data());
    mainWindow.setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(modelManager.data());
    setSegmentationCodesOnGalleryObjectModelModel();
    mainWindow.setGalleryObjectModelModel(galleryObjectModelModel);
    mainWindow.setModelManager(modelManager.data());

    connect(&mainWindow, &MainWindow::posePredictionRequested,
            this, &MainController::onPosePredictionRequested);
    connect(&mainWindow, &MainWindow::posePredictionRequestedForImages,
            this, &MainController::onPosePredictionRequestedForImages);


    mainWindow.onInitializationCompleted();
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    galleryObjectModelModel->setSegmentationCodesForObjectModels(currentSettings->getSegmentationCodes());
}

void MainController::showView() {
    mainWindow.show();
    mainWindow.raise();
}

void MainController::onPosePredictionRequested() {
    performPosePredictionForImages(QVector<ImagePtr>() << mainWindow.getCurrentlyViewedImage());
}

void MainController::onPosePredictionRequestedForImages(const QVector<ImagePtr> &images) {
    performPosePredictionForImages(images);
}

void MainController::performPosePredictionForImages(const QVector<ImagePtr> &images) {
    if (currentSettings->getSegmentationImagesPath() == "") {
        mainWindow.displayWarning("Segmentation images path not set", "The path to the segmentation "
                                                                      "images has to "
                                                   "be set in order to run network prediction.");
        return;
    }
    if (networkController.isNull()) {
        networkController.reset(
                    new NeuralNetworkController(currentSettings->getPythonInterpreterPath(),
                                                currentSettings->getTrainingScriptPath(),
                                                currentSettings->getInferenceScriptPath()));
        connect(networkController.data(), &NeuralNetworkController::inferenceFinished,
                this, &MainController::onNetworkInferenceFinished);
    } else {
        networkController->setPythonInterpreter(currentSettings->getPythonInterpreterPath());
        networkController->setTrainPythonScript(currentSettings->getTrainingScriptPath());
        networkController->setInferencePythonScript(currentSettings->getInferenceScriptPath());
    }
    networkController->setImages(images);
    networkController->setPosesFilePath(currentSettings->getPosesFilePath());
    networkController->setImagesPath(currentSettings->getImagesPath());
    networkController->setSegmentationImagesPath(currentSettings->getSegmentationImagesPath());
    networkController->inference(currentSettings->getNetworkConfigPath());
}

void MainController::onNetworkTrainingFinished() {
    // Nothing to do here, training not yet implemented
}

void MainController::onNetworkInferenceFinished() {
    mainWindow.hideNetworkProgressView();
}

void MainController::onFailedToLoadImages(const QString &message){
    QString imagesPath = currentSettings->getImagesPath();
    if (imagesPath != "." && imagesPath != "") {
        //! "." is the default path and will likely never be used as image path, thus
        //! do not display any warning - otherwise the warning would pop up three times
        //! on startup, because the program tries to load images when setting the
        //! images extension, the paths, etc.
        mainWindow.displayWarning("Error loading images", message);
    }
}

void MainController::onFailedToLoadObjectModels(const QString &message) {
    mainWindow.displayWarning("Error loading object models", message);
}

void MainController::onFailedToLoadPoses(const QString &message) {
    mainWindow.displayWarning("Error loading poses", message);
}

void MainController::onFailedToPersistPose(const QString &message) {
    mainWindow.displayWarning("Error persisting pose", message);
    mainWindow.abortPoseCreation();
}

void MainController::onSettingsChanged(const QString &identifier) {
    currentSettings = settingsStore->loadPreferencesByIdentifier(identifier);
    // Load and store strategy updates itself
    setSegmentationCodesOnGalleryObjectModelModel();
    poseCreator->reset();
}
