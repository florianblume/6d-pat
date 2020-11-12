#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"

#include <QSplashScreen>
#include <QFontDatabase>

MainController::MainController(int &argc, char **argv, int) : QApplication(argc, argv) {
}

MainController::~MainController() {
}

int MainController::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

    initialize();

    QFontDatabase::addApplicationFont(":/fonts/fontawesome-4.7.0.ttf");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(0);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);

    QTimer::singleShot(1500, &splash, &QWidget::close);
    QTimer::singleShot(1500, this, &MainController::showView);
    return QApplication::exec();
}

void MainController::initialize() {
    currentSettings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    settingsStore.reset(new SettingsStore());
    strategy.reset(new JsonLoadAndStoreStrategy(settingsStore.data(),
                                                settingsIdentifier));
    modelManager.reset(new CachingModelManager(*strategy.data()));
    modelManager->reload();
    poseRecoverer.reset(new PoseRecoverer(modelManager.get()));
    connect(settingsStore.data(), &SettingsStore::settingsChanged,
            this, &MainController::onSettingsChanged);
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
