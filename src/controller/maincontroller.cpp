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
    modelManagerThread = new QThread;
    poseRecovererThread = new QThread;
    connect(settingsStore.data(), &SettingsStore::settingsChanged,
            this, &MainController::onSettingsChanged);
    modelManager->moveToThread(modelManagerThread);
    modelManager->reload();
    poseRecoverer->moveToThread(poseRecovererThread);
    mainWindow.reset(new MainWindow(0, modelManager.get(), settingsStore.get(), settingsIdentifier, poseRecoverer.get()));
    poseEditingModel.reset(new PoseEditingController(Q_NULLPTR, modelManager.get(), mainWindow.get()));
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
