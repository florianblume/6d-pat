#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "view/misc/displayhelper.hpp"
#include "view/settings/settingsdialog.hpp"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLayout>

//! The main window of the application that holds the individual components.<
MainWindow::MainWindow(QWidget *parent,
                       ModelManager *modelManager,
                       SettingsStore *settingsStore,
                       const QString &settingsIdentifier,
                       PoseRecoveringController *poseRecoverer) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settingsStore(settingsStore),
    modelManager(modelManager),
    poseRecoverer(poseRecoverer),
    settingsIdentifier(settingsIdentifier) {

    ui->setupUi(this);

    dataLoadingProgressDialog = new QProgressDialog;
    dataLoadingProgressDialog->setRange(0, 0);
    dataLoadingProgressDialog->setCancelButton(Q_NULLPTR);
    dataLoadingProgressDialog->setLabelText("Loading...");
    dataLoadingProgressDialog->setModal(true);
    dataLoadingProgressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    dataLoadingProgressDialog->setFixedSize(dataLoadingProgressDialog->size());
    dataLoadingProgressDialog->show();

    statusBar()->addPermanentWidget(statusBarLabel, 1);
    setStatusBarText(QString("Loading..."));


    connect(settingsStore, &SettingsStore::settingsChanged,
            this, &MainWindow::onSettingsChanged);

    galleryImageModel = new GalleryImageModel(modelManager);
    setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(modelManager);
    setGalleryObjectModelModel(galleryObjectModelModel);

    setPathsOnGalleriesAndBreadcrumbs();

    // If the selected image changes, we also need to cancel any started creation of a pose
    connect(ui->galleryLeft, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);
    connect(ui->galleryRight, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);

    // Prevent crashing of the program when the user clicks too quickly in the
    // object models gallery
    connect(ui->poseEditor, &PoseEditor::loadingObjectModel,
            ui->galleryRight, &Gallery::disable);
    connect(ui->poseEditor, &PoseEditor::objectModelLoaded,
            ui->galleryRight, &Gallery::enable);

    setStatusBarText("Ready.");

    dataLoadingProgressDialog->close();
}

MainWindow::~MainWindow() {
    delete ui;
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

void MainWindow::handleClosingProgram() {
    Q_EMIT closingProgram();
    writeSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    handleClosingProgram();
    event->accept();
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

void MainWindow::abortPoseCreation() {
    onActionReloadViewsTriggered();
    Q_EMIT poseCreationAborted();
}

void MainWindow::showEvent(QShowEvent *e) {
    if (!showInitialized) {
        readSettings();
    }
    showInitialized = true;
    QMainWindow::showEvent(e);
}

void MainWindow::setStatusBarText(const QString& text) {
    statusBarLabel->setText(text);
}

PoseViewer *MainWindow::poseViewer() {
    return ui->poseViewer;
}

PoseEditor *MainWindow::poseEditor() {
    return ui->poseEditor;
}

GalleryObjectModels *MainWindow::galleryObjectModels() {
    return dynamic_cast<GalleryObjectModels*>(ui->galleryRight);
}

Gallery *MainWindow::galleryImages() {
    return ui->galleryLeft;
}

void MainWindow::onImagesPathChangedByNavigation(const QString &path) {
    QSharedPointer<Settings> preferences = settingsStore->loadPreferencesByIdentifier("default");
    preferences->setImagesPath(path);
    settingsStore->savePreferences(*preferences);
}

void MainWindow::onObjectModelsPathChangedByNavigation(const QString &path) {
    QSharedPointer<Settings> preferences = settingsStore->loadPreferencesByIdentifier("default");
    preferences->setObjectModelsPath(path);
    settingsStore->savePreferences(*preferences);
}

void MainWindow::displayWarning(const QString &title, const QString &text) {
    QMessageBox::warning(this, title, text);
}

bool MainWindow::showSaveUnsavedChangesDialog() {
    DisplayHelper::QMessageBoxPtr messageBox = DisplayHelper::messageBox(this,
                                                                         QMessageBox::Warning,
                                                                        "Unsaved pose modifications",
                                                                         "You have unsaved modifications of the currently edited pose."
                                                                         " The action you just performed would discard these modifications. "
                                                                         "Save them now?",
                                                                         "Yes", QMessageBox::YesRole,
                                                                         "No", QMessageBox::NoRole);
    return QMessageBox::Yes == messageBox->exec();
}

void MainWindow::setPathsOnGalleriesAndBreadcrumbs() {
    QSharedPointer<Settings> settings = settingsStore->loadPreferencesByIdentifier(settingsIdentifier);
    galleryObjectModelModel->setSegmentationCodesForObjectModels(settings->segmentationCodes());
    ui->breadcrumbLeft->setPathToShow(settings->imagesPath());
    ui->breadcrumbRight->setPathToShow(settings->objectModelsPath());
    ui->navigationLeft->setPathToOpen(QString(settings->imagesPath()));
    ui->navigationRight->setPathToOpen(QString(settings->objectModelsPath()));
}

void MainWindow::onSettingsChanged(SettingsPtr settings) {
    if (settingsIdentifier != settings->identifier()) {
        return;
    }
    galleryObjectModelModel->setSegmentationCodesForObjectModels(settings->segmentationCodes());
    setPathsOnGalleriesAndBreadcrumbs();
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
                                "network."
                                "\n"
                                "\n"
                                "© Florian Blume, 2020"));
    about.setStandardButtons(QMessageBox::Ok);
    QPixmap icon(":/images/about.png");
    about.setIconPixmap(icon);
    about.setDefaultButton(QMessageBox::Ok);
    about.show();
    about.exec();
}

void MainWindow::onActionExitTriggered() {
    handleClosingProgram();
    QApplication::quit();
}

void MainWindow::onActionSettingsTriggered() {
    SettingsDialog* settingsDialog = new SettingsDialog(this);
    settingsDialog->setPreferencesStoreAndObjectModels(settingsStore,
                                                       "default",
                                                       modelManager->objectModels());
    settingsDialog->show();
}

void MainWindow::onActionAbortCreationTriggered() {
    setStatusBarText("Ready.");
    poseRecoverer->reset();
}

void MainWindow::onActionReloadViewsTriggered() {
    Q_EMIT reloadingViews();
    // Need to reload in case some data changed and we haven't picked it up
    modelManager->reload();
    // Reset just to be sure
    ui->poseEditor->reset();
    ui->poseViewer->reset();
    ui->galleryLeft->reset();
    ui->galleryRight->reset();
    setStatusBarText("Ready.");
}

void MainWindow::onModelManagerStateChanged(ModelManager::State state) {
    if (state == ModelManager::Loading) {
        dataLoadingProgressDialog->show();
    } else {
        dataLoadingProgressDialog->close();
    }
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
