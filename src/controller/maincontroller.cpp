#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSettings>
#include <QDir>
#include <iostream>

MainController::MainController() {
}

MainController::~MainController() {
}

void MainController::initialize() {
    currentSettings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    settingsStore.reset(new SettingsStore());
    strategy.reset(new JsonLoadAndStoreStrategy(settingsStore.data(),
                                                settingsIdentifier));
    modelManager.reset(new CachingModelManager(*strategy.data()));
    poseRecoverer.reset(new PoseRecoverer(modelManager.get()));
    poseEditingModel.reset(new PoseEditingModel);
    modelManagerThread = new QThread;
    poseRecovererThread = new QThread;
    connect(settingsStore.data(), &SettingsStore::settingsChanged,
            this, &MainController::onSettingsChanged);
    modelManager->moveToThread(modelManagerThread);
    modelManager->reload();
    poseRecoverer->moveToThread(poseRecovererThread);
    mainWindow.reset(new MainWindow(0, modelManager.get(), settingsStore.get(), settingsIdentifier, poseRecoverer.get()));

    // Connect the PoseEditor and PoseViewer to the PoseEditingController
    connect(poseEditingModel.get(), &PoseEditingModel::selectedPoseChanged,
            mainWindow->poseViewer(), &PoseViewer::selectPose);
    connect(poseEditingModel.get(), &PoseEditingModel::selectedPoseChanged,
            mainWindow->poseEditor(), &PoseEditor::selectPose);
    connect(mainWindow->poseViewer(), &PoseViewer::poseSelected,
            poseEditingModel.get(), &PoseEditingModel::selectPose);
    connect(mainWindow->poseEditor(), &PoseEditor::poseSelected,
            poseEditingModel.get(), &PoseEditingModel::selectPose);
}

void MainController::showView() {
    mainWindow->show();
    mainWindow->raise();
}

void MainController::onSettingsChanged(const QString &identifier) {
    if (currentSettings->getIdentifier() == identifier) {
        poseRecoverer->reset();
    }
}
