#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"

#include <QSplashScreen>
#include <QFontDatabase>
#include <QFile>
#include <QApplication>

MainController::MainController(int &argc, char **argv, int)
    : QApplication(argc, argv)
    , m_modelManagerThread(new QThread) {
}

MainController::~MainController() {
}

int MainController::exec() {
    qSetMessagePattern("[%{function}] (%{type}): %{message}");

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

    return QApplication::exec();
}

void MainController::initialize() {
    m_settingsStore.reset(new SettingsStore(m_settingsIdentifier));
    Settings tmp(*m_settingsStore->currentSettings());
    m_currentSettings.reset(new Settings(tmp));
    m_strategy.reset(new JsonLoadAndStoreStrategy());
    m_strategy->moveToThread(m_modelManagerThread);
    setPathsOnLoadAndStoreStrategy();
    m_modelManager.reset(new CachingModelManager(*m_strategy.data()));
    connect(this, &MainController::reloadingData,
            m_modelManager.get(), &ModelManager::reload);
    m_modelManager->moveToThread(m_modelManagerThread);
    m_modelManagerThread->start();
    connect(m_settingsStore.data(), &SettingsStore::currentSettingsChanged,
            this, &MainController::onSettingsChanged);
    m_mainWindow.reset(new MainWindow(0, m_modelManager.get(), m_settingsStore.get()));
    connect(m_mainWindow.get(), &MainWindow::reloadingViews,
            this, &MainController::onReloadViewsRequested);
    connect(m_modelManager.get(), &ModelManager::stateChanged,
            this, &MainController::onModelManagerStateChanged);
    m_mainWindow->poseViewer()->setSettingsStore(m_settingsStore.get());
    showView();
    QPixmap pixmap(":/images/splash.png");
    m_splashScreen = new QSplashScreen(pixmap);
    m_splashScreen->show();
    m_poseEditingModel.reset(new PosesEditingController(Q_NULLPTR, m_modelManager.get(), m_mainWindow.get()));
    // This makes the ModelManager load data don't call it before creating the MainWindow as we
    // want to show the progress loading view in the ModelManager state change callback
    Q_EMIT reloadingData();
}

void MainController::showView() {
    m_mainWindow->show();
    m_mainWindow->raise();
    m_mainWindow->repaint();
}

void MainController::setPathsOnLoadAndStoreStrategy() {
    m_strategy->setImagesPath(m_currentSettings->imagesPath());
    m_strategy->setObjectModelsPath(m_currentSettings->objectModelsPath());
    m_strategy->setPosesFilePath(m_currentSettings->posesFilePath());
    m_strategy->setSegmentationImagesPath(m_currentSettings->segmentationImagesPath());
}

void MainController::onSettingsChanged(SettingsPtr settings) {
    bool changed = m_currentSettings->imagesPath() != settings->imagesPath() ||
                   m_currentSettings->segmentationImagesPath() != settings->segmentationImagesPath() ||
                   m_currentSettings->objectModelsPath() != settings->objectModelsPath() ||
                   m_currentSettings->posesFilePath() != settings->posesFilePath();
    m_strategy->setImagesPath(m_currentSettings->imagesPath());
    m_strategy->setObjectModelsPath(m_currentSettings->objectModelsPath());
    m_strategy->setPosesFilePath(m_currentSettings->posesFilePath());
    m_strategy->setSegmentationImagesPath(m_currentSettings->segmentationImagesPath());
    if (changed) {
        Q_EMIT reloadingData();
    }
    // We need to reset the stored currentSettings like this here because we need settings
    // that are independend of the settings of the settings store because those might get
    // altered but we want to be able to compare if something has changed
    Settings tmp(*settings);
    m_currentSettings.reset(new Settings(tmp));
}

void MainController::onReloadViewsRequested() {
    Q_EMIT reloadingData();
}

void MainController::onModelManagerStateChanged(ModelManager::State state) {
    if (state == ModelManager::Ready && !m_initialized) {
        QTimer::singleShot(1000, m_splashScreen, &QWidget::close);
        m_initialized = true;
    }
}
