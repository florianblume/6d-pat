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
    poseCreator.reset(new PoseCreator(0, &modelManager));
    // Whenever the user clicks the create button in the pose editor we need to reset
    // the controller as well
    connect(&modelManager, SIGNAL(poseAdded(QString)),
            this, SLOT(resetPoseCreation()));
    connect(&modelManager, SIGNAL(poseDeleted(QString)),
            this, SLOT(resetPoseCreation()));
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
    strategy.setPosesFilePath(currentPreferences->getPosesFilePath());
    strategy.setSegmentationImagesPath(currentPreferences->getSegmentationImagesPath());
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

    // ## Pose stuff

    // Delegation of pose creator actions to the window
    connect(poseCreator.get(), SIGNAL(poseCreationAborted()),
            &mainWindow, SLOT(onPoseCreationReset()));
    connect(poseCreator.get(), &PoseCreator::posePointStarted,
            &mainWindow, &MainWindow::onPosePointStarted);
    connect(poseCreator.get(), &PoseCreator::posePointFinished,
            &mainWindow, &MainWindow::onPosePointFinished);

    // Delegate of user interactions to the controller
    connect(&mainWindow, SIGNAL(poseCreationInterrupted()),
            this, SLOT(onPoseCreationInterrupted()));
    connect(&mainWindow, SIGNAL(poseCreationAborted()),
            this, SLOT(onPoseCreationAborted()));
    connect(&mainWindow, SIGNAL(requestPoseCreation()),
            this, SLOT(onPoseCreationRequested()));

    connect(&mainWindow, &MainWindow::posePredictionRequested,
            this, &MainController::onPosePredictionRequested);
    connect(&mainWindow, &MainWindow::posePredictionRequestedForImages,
            this, &MainController::onPosePredictionRequestedForImages);


    mainWindow.onInitializationCompleted();
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    galleryObjectModelModel->setSegmentationCodesForObjectModels(currentPreferences->getSegmentationCodes());
}

void MainController::onImageClicked(Image* image, QPoint position) {
    if (poseCreator->getState() != PoseCreator::State::PosePointStarted) {
        // We can set the image here everytime, if it differs from the previously one, the creator will
        // automatically reset the points etc.
        poseCreator->setImage(image);
        poseCreator->startPosePoint(position);
    }
}

void MainController::onObjectModelClicked(ObjectModel* objectModel, QVector3D position) {
    if (poseCreator->isImageSet() && poseCreator->getState() ==
                                               PoseCreator::State::PosePointStarted) {
        poseCreator->setObjectModel(objectModel);
        poseCreator->finishPosePoint(position);
    }
}


void MainController::onPoseCreationInterrupted() {
    // nothing to do here
}

void MainController::onPoseCreationAborted() {
    resetPoseCreation();
}

void MainController::onImagePathChanged(const QString &newPath) {
    this->strategy.setImagesPath(newPath);
    this->currentPreferences->setImagesPath(newPath);
    resetPoseCreation();
}

void MainController::onObjectModelsPathChanged(const QString &newPath) {
    this->strategy.setObjectModelsPath(newPath);
    this->currentPreferences->setObjectModelsPath(newPath);
    resetPoseCreation();
}

void MainController::showView() {
    mainWindow.show();
    mainWindow.raise();
}

void MainController::resetPoseCreation() {
    poseCreator->abortCreation();
}

void MainController::onPoseCreationRequested() {
    // The user can't request this before all the requirements are met because the creat button
    // is not enabled earlier
    poseCreator->createPose();
}

void MainController::onPosePredictionRequested() {
    performPosePredictionForImages(QList<Image>() << *mainWindow.getCurrentlyViewedImage());
}

void MainController::onPosePredictionRequestedForImages(QList<Image> images) {
    performPosePredictionForImages(images);
}

void MainController::performPosePredictionForImages(QList<Image> images) {
    if (currentPreferences->getSegmentationImagesPath() == "") {
        mainWindow.displayWarning("Segmentation images path not set", "The path to the segmentation "
                                                                      "images has to "
                                                   "be set in order to run network prediction.");
        return;
    }
    if (networkController.isNull()) {
        networkController.reset(
                    new NeuralNetworkController(currentPreferences->getPythonInterpreterPath(),
                                                currentPreferences->getTrainingScriptPath(),
                                                currentPreferences->getInferenceScriptPath()));
        connect(networkController.data(), &NeuralNetworkController::inferenceFinished,
                this, &MainController::onNetworkInferenceFinished);
    } else {
        networkController->setPythonInterpreter(currentPreferences->getPythonInterpreterPath());
        networkController->setTrainPythonScript(currentPreferences->getTrainingScriptPath());
        networkController->setInferencePythonScript(currentPreferences->getInferenceScriptPath());
    }
    networkController->setImages(images.toVector());
    networkController->setPosesFilePath(strategy.getPosesFilePath().path());
    networkController->setImagesPath(currentPreferences->getImagesPath());
    networkController->setSegmentationImagesPath(currentPreferences->getSegmentationImagesPath());
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
    if (imagesPath != "." && imagesPath != "") {
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
    if (currentPreferences->getPosesFilePath().compare(strategy.getPosesFilePath().path()) != 0)
        strategy.setPosesFilePath(currentPreferences->getPosesFilePath());
    if (currentPreferences->getSegmentationImagesPath().compare(strategy.getSegmentationImagesPath().path()) != 0)
        strategy.setSegmentationImagesPath(currentPreferences->getSegmentationImagesPath());
    setSegmentationCodesOnGalleryObjectModelModel();
    poseCreator->abortCreation();
}
