#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"

#include <QSplashScreen>
#include <QFontDatabase>
#include <QFile>
#include <QApplication>

MainController::MainController(int &argc, char **argv, int)
    : QApplication(argc, argv)
    , modelManagerThread(new QThread) {
}

MainController::~MainController() {
}

int MainController::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

    QPixmap pixmap(":/images/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

    QFile stream(":/stylesheets/light.qss");
    stream.open(QIODevice::ReadOnly);
    qApp->setStyleSheet(stream.readAll());

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
    currentSettings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    strategy.reset(new JsonLoadAndStoreStrategy());
    strategy->moveToThread(modelManagerThread);
    setPathsOnLoadAndStoreStrategy();
    modelManager.reset(new CachingModelManager(*strategy.data()));
    connect(this, &MainController::reloadingData,
            modelManager.get(), &ModelManager::reload);
    modelManager->moveToThread(modelManagerThread);
    modelManagerThread->start();
    connect(settingsStore.data(), &SettingsStore::settingsChanged,
            this, &MainController::onSettingsChanged);
    mainWindow.reset(new MainWindow(0, modelManager.get(), settingsStore.get(), settingsIdentifier));
    connect(mainWindow.get(), &MainWindow::reloadingViews,
            this, &MainController::onReloadViewsRequested);
    mainWindow->poseViewer()->setSettingsStore(settingsStore.get());
    poseEditingModel.reset(new PosesEditingController(Q_NULLPTR, modelManager.get(), mainWindow.get()));
    // This makes the ModelManager load data don't call it before creating the MainWindow as we
    // want to show the progress loading view in the ModelManager state change callback
    Q_EMIT reloadingData();
}

void MainController::showView() {
    mainWindow->show();
    mainWindow->raise();
    mainWindow->repaint();
}

void MainController::setPathsOnLoadAndStoreStrategy() {
    strategy->setImagesPath(currentSettings->imagesPath());
    strategy->setObjectModelsPath(currentSettings->objectModelsPath());
    strategy->setPosesFilePath(currentSettings->posesFilePath());
    strategy->setSegmentationImagesPath(currentSettings->segmentationImagesPath());
}

void MainController::onSettingsChanged(SettingsPtr settings) {
    currentSettings = settings;
    setPathsOnLoadAndStoreStrategy();
    Q_EMIT reloadingData();
}

void MainController::onReloadViewsRequested() {
    Q_EMIT reloadingData();
}
