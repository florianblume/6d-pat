#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSettings>
#include <QDir>
#include <iostream>

MainController::MainController()
    : settingsStore(new SettingsStore())
    , strategy(new JsonLoadAndStoreStrategy(settingsStore.data(),
                                            settingsIdentifier))
    , modelManager(new CachingModelManager(*strategy.data()))
    , poseRecoverer(new PoseRecoverer(modelManager.get()))
    , mainWindow(0, modelManager.get(), settingsStore.get(), settingsIdentifier, poseRecoverer.get()) {
    connect(settingsStore.data(), &SettingsStore::settingsChanged,
            this, &MainController::onSettingsChanged);
}

MainController::~MainController() {
}

void MainController::initialize() {
    currentSettings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
}

void MainController::showView() {
    mainWindow.show();
    mainWindow.raise();
}

void MainController::onSettingsChanged(const QString &identifier) {
    if (currentSettings->getIdentifier() == identifier) {
        poseRecoverer->reset();
    }
}
