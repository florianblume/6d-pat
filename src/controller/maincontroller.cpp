#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.hpp"

#include <QSplashScreen>
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

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(0);
    format.setSamples(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 3);
    QSurfaceFormat::setDefaultFormat(format);

    initialize();

    return QApplication::exec();
}

void MainController::initialize() {
    m_settingsStore.reset(new SettingsStore(m_settingsIdentifier));
    Settings tmp(*m_settingsStore->currentSettings());
    m_currentSettings.reset(new Settings(tmp));

    m_strategy.reset(new JsonLoadAndStoreStrategy());
    // Move the strategy to a new thread to allow threadded data loading
    // This also means that we have to call the strategy's methods
    // through signals and slots, directly calling them does not do
    // anything threadded
    m_strategy->moveToThread(m_modelManagerThread);
    m_strategy->setImagesPath(m_currentSettings->imagesPath());
    m_strategy->setObjectModelsPath(m_currentSettings->objectModelsPath());
    m_strategy->setPosesFilePath(m_currentSettings->posesFilePath());
    m_strategy->setSegmentationImagesPath(m_currentSettings->segmentationImagesPath());

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
    m_mainWindow->poseEditor()->setSettingsStore(m_settingsStore.get());

    showView();

    m_splashScreen = new SplashScreen();
    // Make it an infinite progress bar
    m_splashScreen->setMaximum(0);
    m_splashScreen->setMinimum(0);
    m_splashScreen->show();
    QTimer::singleShot(500, [this](){
        m_splashScreen->showProgressBar(true);
    });

    // Call here since we need the model manager and the main window
    m_poseEditingModel.reset(new PosesEditingController(Q_NULLPTR, m_modelManager.get(), m_mainWindow.get()));

    // This makes the ModelManager load data - don't call it before creating the MainWindow as we
    // want to show the progress loading view in the ModelManager state change callback
    // Emit the signal to load data threadded, directly calling the methods
    // does not do anything threadded
    Q_EMIT reloadingData();
}

void MainController::showView() {
    m_mainWindow->show();
    m_mainWindow->raise();
    m_mainWindow->repaint();
}

void MainController::onSettingsChanged(SettingsPtr settings) {
    bool changed = m_currentSettings->imagesPath() != settings->imagesPath() ||
                   m_currentSettings->segmentationImagesPath() != settings->segmentationImagesPath() ||
                   m_currentSettings->objectModelsPath() != settings->objectModelsPath() ||
                   m_currentSettings->posesFilePath() != settings->posesFilePath();
    m_strategy->setImagesPath(settings->imagesPath());
    m_strategy->setObjectModelsPath(settings->objectModelsPath());
    m_strategy->setPosesFilePath(settings->posesFilePath());
    m_strategy->setSegmentationImagesPath(settings->segmentationImagesPath());
    if (changed) {
        // Emit the signal to load data threadded, directly calling the methods
        // does not do anything threadded
        Q_EMIT reloadingData();
    }
    // We need to reset the stored currentSettings like this here because we need settings
    // that are independend of the settings of the settings store because those might get
    // altered but we want to be able to compare if something has changed
    Settings tmp(*settings);
    m_currentSettings.reset(new Settings(tmp));
}

void MainController::onReloadViewsRequested() {
    // Emit the signal to load data threadded, directly calling the methods
    // does not do anything threadded
    Q_EMIT reloadingData();
}

void MainController::onModelManagerStateChanged(ModelManager::State state, LoadAndStoreStrategy::Error /*error*/) {
    // First hide the progress viwe (e.g. for Ready or ErrorOccured)
    m_mainWindow->showDataLoadingProgressView(false);
    if (state == ModelManager::Ready && !m_initialized) {
        // First state change means model manager is ready but we are not initialized yet
        QTimer::singleShot(1000, m_splashScreen, &QWidget::close);
        m_initialized = true;
    } else if (state == ModelManager::Loading) {
        if (m_initialized) {
            // Only show when initialized, otherwise we're showing the splash screen
            // which shows a progress bar
            m_mainWindow->showDataLoadingProgressView(true);
        }
    }
}
