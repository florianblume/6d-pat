#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "view/settings/settingsdialog.hpp"
#include "view/neuralnetworkdialog/neuralnetworkdialog.hpp"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLayout>

//! The main window of the application that holds the individual components.<
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();
    statusBar()->addPermanentWidget(statusBarLabel, 1);
    setStatusBarText(QString("Loading..."));

    // If the selected image changes, we also need to cancel any started creation of a pose
    connect(ui->galleryLeft, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);
    connect(ui->galleryRight, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onInitializationStarted() {
    setStatusBarText("Initializing...");
}

void MainWindow::onInitializationCompleted() {
    setStatusBarText("Ready.");
}

//! This function persistently stores settings of the application.
void MainWindow::writeSettings() {
    QSettings settings(MainWindow::SETTINGS_NAME, MainWindow::SETTINGS_PROGRAM_NAME);

    settings.beginGroup(MainWindow::SETTINGS_GROUP_NAME);
    settings.setValue(MainWindow::WINDOW_IS_FULLSCREEN_KEY, isMaximized());
    settings.setValue(MainWindow::WINDOW_SIZE_KEY, size());
    settings.setValue(MainWindow::WINDOW_POSITION_KEY, pos());

    //! We store the aspect ratios of the splitters for convenience of the user
    QList<int> splitterMainSizes = ui->splitterMain->sizes();
    settings.setValue(MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY, splitterMainSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY, splitterMainSizes.at(1));
    QList<int> splitterLefSizes = ui->splitterLeft->sizes();
    settings.setValue(MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY, splitterLefSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY, splitterLefSizes.at(1));
    QList<int> splitterRightSizes = ui->splitterRight->sizes();
    settings.setValue(MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY, splitterRightSizes.at(0));
    settings.setValue(MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY, splitterRightSizes.at(1));
    settings.endGroup();
}

//! This function reads the persisted settings and restores the program's state.
void MainWindow::readSettings() {
    QSettings settings(MainWindow::SETTINGS_NAME, MainWindow::SETTINGS_PROGRAM_NAME);

    settings.beginGroup(MainWindow::SETTINGS_GROUP_NAME);
    if (settings.value(MainWindow::WINDOW_IS_FULLSCREEN_KEY).toBool()) {
        showMaximized();
    }
    resize(settings.value(MainWindow::WINDOW_SIZE_KEY, QSize(600, 400)).toSize());
    move(settings.value(MainWindow::WINDOW_POSITION_KEY, QPoint(200, 200)).toPoint());
    QList<int> splitterMainSizes;
    splitterMainSizes.append(settings.value(MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY, 300).toInt());
    splitterMainSizes.append(settings.value(MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY, 300).toInt());
    ui->splitterMain->setSizes(splitterMainSizes);
    QList<int> splitterLeftSizes;
    splitterLeftSizes.append(settings.value(MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY, 200).toInt());
    splitterLeftSizes.append(settings.value(MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY, 400).toInt());
    ui->splitterLeft->setSizes(splitterLeftSizes);
    QList<int> splitterRightSizes;
    splitterRightSizes.append(settings.value(MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY, 200).toInt());
    splitterRightSizes.append(settings.value(MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY, 400).toInt());
    ui->splitterRight->setSizes(splitterRightSizes);
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

void MainWindow::setPathOnLeftBreadcrumbView(const QString &pathToShow) {
    ui->breadcrumbLeft->setPathToShow(pathToShow);
}

void MainWindow::setPathOnRightBreadcrumbView(const QString &pathToShow) {
    ui->breadcrumbRight->setPathToShow(pathToShow);
}

void MainWindow::setPathOnLeftNavigationControls(const QString &path) {
    ui->navigationLeft->setPathToOpen(path);
}

void MainWindow::setPathOnRightNavigationControls(const QString &path) {
    ui->navigationRight->setPathToOpen(path);
}

void MainWindow::setGalleryImageModel(GalleryImageModel* model) {
    this->ui->galleryLeft->setModel(model);
}

void MainWindow::setGalleryObjectModelModel(GalleryObjectModelModel* model) {
    this->ui->galleryRight->setModel(model);
    // We connect the model that displays the 3D model objects to the selected image so that the model
    // can display the respective models
    connect(ui->galleryLeft, &Gallery::selectedItemChanged, model, &GalleryObjectModelModel::onSelectedImageChanged);
    // To update the listview of the 3D models we call the reset method when the model receives the signal
    // that the selection of images on the left has been changed
    connect(model, &GalleryObjectModelModel::displayedObjectModelsChanged, ui->galleryRight, &Gallery::reset);
}

void MainWindow::setModelManager(ModelManager* modelManager) {
    this->modelManager = modelManager;
    ui->poseViewer->setModelManager(modelManager);
    ui->poseEditor->setModelManager(modelManager);
}

void MainWindow::resetPoseViewer() {
    ui->poseViewer->reset();
}

void MainWindow::abortPoseCreation() {
    onActionReloadViewsTriggered();
    Q_EMIT poseCreationAborted();
}

void MainWindow::setPreferencesStore(SettingsStore *preferencesStore) {
    if (this->preferencesStore) {
        disconnect(this->preferencesStore, SIGNAL(preferencesChanged(QString)),
                   this, SLOT(onSettingsChanged(QString)));
    }
    this->preferencesStore = preferencesStore;
    connect(preferencesStore, SIGNAL(settingsChanged(QString)),
            this, SLOT(onSettingsChanged(QString)));
}

ImagePtr MainWindow::getCurrentlyViewedImage() {
    return ui->poseViewer->currentlyViewedImage();
}

void MainWindow::resizeEvent(QResizeEvent *) {
    if (!networkProgressView.isNull()) {
        networkProgressView->setGeometry(0, 0, this->width(), this->height());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* /* event */) {
    if (poseCreationInProgress) {
        // Reset pose creation because the user clicked anywhere
        onPoseCreationReset();
        Q_EMIT poseCreationAborted();
    }
}

void MainWindow::setStatusBarText(const QString& text) {
    statusBarLabel->setText(text);
}

void MainWindow::onSelectedObjectModelChanged(int index) {
    QVector<ObjectModelPtr> objectModels = modelManager->getObjectModels();
    Q_ASSERT(index >= 0 && index < objectModels.size());
    Q_EMIT selectedObjectModelChanged(objectModels[index]);
    onPoseCreationReset();
}

void MainWindow::onSelectedImageChanged(int index) {
    QVector<ImagePtr> images = modelManager->getImages();
    Q_ASSERT(index >= 0 && index < images.size());
    Q_EMIT selectedImageChanged(images[index]);
    onPoseCreationReset();
}

void MainWindow::onImagesPathChangedByNavigation(const QString &path) {
    QSharedPointer<Settings> preferences = preferencesStore->loadPreferencesByIdentifier("default");
    preferences->setImagesPath(path);
    preferencesStore->savePreferences(preferences.data());
}

void MainWindow::onObjectModelsPathChangedByNavigation(const QString &path) {
    QSharedPointer<Settings> preferences = preferencesStore->loadPreferencesByIdentifier("default");
    preferences->setObjectModelsPath(path);
    preferencesStore->savePreferences(preferences.data());
}

void MainWindow::displayWarning(const QString &title, const QString &text) {
    QMessageBox::warning(this, title, text);
}

void MainWindow::onPoseCreated() {
}

void MainWindow::onPoseCreationReset() {
    setStatusBarText("Ready.");
}

void MainWindow::onPoseCreationRequested() {
}

void MainWindow::hideNetworkProgressView() {
    networkProgressView->hide();
    QApplication::restoreOverrideCursor();
}

void MainWindow::setPoseRecoverer(PoseRecoverer *poseRecoverer)
{
    this->poseRecoverer = poseRecoverer;
    ui->poseViewer->setPoseRecoverer(poseRecoverer);
    ui->poseEditor->setPoseRecoverer(poseRecoverer);
}

void MainWindow::onSettingsChanged(const QString &identifier) {
    QSharedPointer<Settings> preferences = preferencesStore->loadPreferencesByIdentifier(identifier);
    setPathOnLeftBreadcrumbView(preferences->getImagesPath());
    setPathOnRightBreadcrumbView(preferences->getObjectModelsPath());
    onPoseCreationReset();
}

void MainWindow::onActionAboutTriggered() {
    QMessageBox about;
    about.setText(tr("What is 6D-PAT?"));
    about.setInformativeText(tr("6D pose annotation tool, "
                                "or shorter 6D-PAT, is a tool "
                                "that allows users to "
                                "position 3D models of objects "
                                "on images. Depending on the "
                                "selected storage options the "
                                "position and rotation are "
                                "persisted for later use. "
                                "The so annotated images can "
                                "be used to e.g. train a neural "
                                "network."));
    about.setStandardButtons(QMessageBox::Ok);
    QPixmap icon(":/images/about.png");
    about.setIconPixmap(icon);   // here is the error
    about.setDefaultButton(QMessageBox::Ok);
    about.show();
    about.exec();
}

void MainWindow::onActionExitTriggered()
{
    QApplication::quit();
}

void MainWindow::onActionSettingsTriggered()
{
    SettingsDialog* settingsDialog = new SettingsDialog(this);
    settingsDialog->setPreferencesStoreAndObjectModels(preferencesStore,
                                                       "default",
                                                       modelManager->getObjectModels());
    settingsDialog->show();
}

void MainWindow::onActionAbortCreationTriggered() {
    setStatusBarText("Ready.");
    poseRecoverer->reset();
}

void MainWindow::onActionReloadViewsTriggered() {
    modelManager->reload();
}

void MainWindow::onActionNetworkPredictTriggered() {
    if (neuralNetworkDialog.isNull()) {
        neuralNetworkDialog.reset(new NeuralNetworkDialog(this, modelManager));
        connect(neuralNetworkDialog.data(), &NeuralNetworkDialog::networkPredictionRequestedForImages,
                this, &MainWindow::onPosePredictionRequestedForImages);
    }
    neuralNetworkDialog->show();
}

void MainWindow::onPosePredictionRequestedForImages(QVector<ImagePtr> images) {
    if (networkProgressView.isNull()) {
        networkProgressView.reset(new NetworkProgressView(this));
    }
    networkProgressView->show();
    networkProgressView->setGeometry(QRect(0, 0, this->width(), this->height()));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit posePredictionRequestedForImages(images);
}

void MainWindow::onPosePredictionRequested() {
    if (networkProgressView.isNull()) {
        networkProgressView.reset(new NetworkProgressView(this));
    }
    networkProgressView->show();
    networkProgressView->setGeometry(QRect(0, 0, this->width(), this->height()));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    Q_EMIT posePredictionRequested();
}

QString MainWindow::SETTINGS_NAME = "FlorettiKonfetti Inc.";
QString MainWindow::SETTINGS_PROGRAM_NAME = "Otiat";
QString MainWindow::SETTINGS_GROUP_NAME = "mainwindow";
QString MainWindow::WINDOW_IS_FULLSCREEN_KEY = "isfullscreen";
QString MainWindow::WINDOW_SIZE_KEY = "windowsize";
QString MainWindow::WINDOW_POSITION_KEY = "windowposition";
QString MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY = "splitterMainLeftSize";
QString MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY = "splitterMainRightSize";
QString MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY = "splitterLeftLeftSize";
QString MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY = "splitterLeftRightSize";
QString MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY = "splitterRightLeftSize";
QString MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY = "splitterRightRightSize";
