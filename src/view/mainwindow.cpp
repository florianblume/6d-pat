#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "view/misc/displayhelper.hpp"
#include "view/settings/settingsdialog.hpp"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLayout>
#include <QGraphicsBlurEffect>

//! The main window of the application that holds the individual components.<
MainWindow::MainWindow(QWidget *parent,
                       ModelManager *modelManager,
                       SettingsStore *settingsStore) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settingsStore(settingsStore),
    modelManager(modelManager) {

    ui->setupUi(this);

    progressDialog = new QProgressDialog;
    progressDialog->setRange(0, 0);
    progressDialog->setCancelButton(Q_NULLPTR);
    progressDialog->setModal(true);
    progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    progressDialog->setFixedSize(progressDialog->size());
    // The controller handles showing the progress dialog externally
    //dataLoadingProgressDialog->show();

    //tutorialScreen = new TutorialScreen(this);
    //tutorialScreen->setGeometry(this->geometry());

    statusBar()->addPermanentWidget(statusBarLabel, 1);
    setStatusBarText(QString("Loading..."));
    connect(modelManager, &ModelManager::stateChanged,
            this, &MainWindow::onModelManagerStateChanged);

    connect(settingsStore, &SettingsStore::currentSettingsChanged,
            this, &MainWindow::onSettingsChanged);

    galleryImageModel = new GalleryImageModel(modelManager);
    setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(modelManager);
    setGalleryObjectModelModel(galleryObjectModelModel);

    setPathsOnGalleriesAndBreadcrumbs();

    connect(ui->breadcrumbLeft, &BreadcrumbView::selectedPathChanged,
            this, &MainWindow::onImagesPathChangedByNavigation);
    connect(ui->breadcrumbRight, &BreadcrumbView::selectedPathChanged,
            this, &MainWindow::onObjectModelsPathChangedByNavigation);

    // If the selected image changes, we also need to cancel any started creation of a pose
    connect(ui->galleryLeft, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);
    connect(ui->galleryRight, &Gallery::selectedItemChanged,
            this, &MainWindow::poseCreationAborted);

    setStatusBarText("Ready.");

    progressDialog->close();
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

void MainWindow::setStatusBarTextStartAddingCorrespondences() {
    setStatusBarText("Start adding correspondences by clicking on either the image or object model.");
}

void MainWindow::setStatusBarText2DPointMissing(int numberOfCorrespondences, int minNumberOfCorrespondences) {
    setStatusBarText("Complete the correspondence by clicking in the image (" +
                     QString::number(numberOfCorrespondences) +
                     "/" +
                     QString::number(minNumberOfCorrespondences) +
                     " correspondences complete).");
}

void MainWindow::setStatusBarText3DPointMissing(int numberOfCorrespondences, int minNumberOfCorrespondences) {
    setStatusBarText("Complete the correspondence by clicking the 3D model (" +
                     QString::number(numberOfCorrespondences) +
                     "/" +
                     QString::number(minNumberOfCorrespondences) +
                     " correspondences complete).");
}

void MainWindow::setStatusBarTextNotEnoughCorrespondences(int numberOfCorrespondences, int minNumberOfCorrespondences) {
    setStatusBarText("Add more correspondences to enable pose recovering (" +
                     QString::number(numberOfCorrespondences) +
                     "/" +
                     QString::number(minNumberOfCorrespondences) +
                     " correspondences complete).");
}

void MainWindow::setStatusBarTextReadyForPoseCreation(int numberOfCorrespondences, int minNumberOfCorrespondences) {
    setStatusBarText("Ready for pose recovering (" +
                     QString::number(numberOfCorrespondences) +
                     "/" +
                     QString::number(minNumberOfCorrespondences) +
                     " correspondences complete).");
}

void MainWindow::showEvent(QShowEvent *e) {
    if (!showInitialized) {
        readSettings();
    }
    showInitialized = true;
    QMainWindow::showEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    QMainWindow::resizeEvent(e);
    if (tutorialScreen) {
        tutorialScreen->setGeometry(0, 0,
                                    this->geometry().width(), this->geometry().height());
        tutorialScreen->setCurrentViewWindow(QRect(100, 100, 100, 100));
    }
}

void MainWindow::setStatusBarText(const QString& text) {
    statusBarLabel->setText(text);
}

void MainWindow::showProgressView(bool show) {
    if (show) {
        progressDialog->show();
        this->setGraphicsEffect(new QGraphicsBlurEffect);
    } else {
        progressDialog->close();
        this->setGraphicsEffect({});
    }
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

void MainWindow::showDataLoadingProgressView(bool show) {
    progressDialog->setLabelText("Loading data...");
    showProgressView(show);
}

void MainWindow::showPoseRecoveringProgressView(bool show) {
    progressDialog->setLabelText("Recovering pose...");
    showProgressView(show);
}

void MainWindow::onImagesPathChangedByNavigation(const QString &path) {
    SettingsPtr settings = settingsStore->currentSettings();
    settings->setImagesPath(path);
    settingsStore->saveCurrentSettings();
}

void MainWindow::onObjectModelsPathChangedByNavigation(const QString &path) {
    SettingsPtr settings = settingsStore->currentSettings();
    settings->setObjectModelsPath(path);
    settingsStore->saveCurrentSettings();
}

void MainWindow::displayWarning(const QString &title, const QString &text) {
    DisplayHelper::QMessageBoxPtr messageBox = DisplayHelper::messageBox(this,
                                                                         QMessageBox::Warning,
                                                                         title,
                                                                         text,
                                                                         "Ok", QMessageBox::AcceptRole);
    messageBox->exec();
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
    SettingsPtr settings = settingsStore->currentSettings();
    galleryObjectModelModel->setSegmentationCodesForObjectModels(settings->segmentationCodes());
    ui->breadcrumbLeft->setCurrentPath(settings->imagesPath());
    ui->breadcrumbRight->setCurrentPath(settings->objectModelsPath());
}

void MainWindow::onSettingsChanged(SettingsPtr settings) {
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
    settingsDialog->setSettingsStoreAndObjectModels(settingsStore,
                                                    modelManager->objectModels());
    settingsDialog->show();
}

void MainWindow::onActionAbortCreationTriggered() {
    setStatusBarText("Ready.");
    Q_EMIT poseCreationAborted();
}

void MainWindow::onActionReloadViewsTriggered() {
    Q_EMIT reloadingViews();
    setStatusBarText("Ready.");
}

void MainWindow::onModelManagerStateChanged(ModelManager::State state, LoadAndStoreStrategy::Error error) {
    if (state == ModelManager::ErrorOccured) {
        QString message("An unkown error occured in the data manager.");
        switch (error) {
            case LoadAndStoreStrategy::Error::None:
                // Cannot occur, just so that there is no warning here
                break;
            case LoadAndStoreStrategy::Error::CamInfoDoesNotExist:
                message = "The camera info file info.json does not exist.";
                break;
            case LoadAndStoreStrategy::Error::ImagesPathDoesNotExist:
                message = "The images path does not exist.";
                break;
            case LoadAndStoreStrategy::Error::SegmentationImagesPathDoesNotExist:
                message = "The segmentation images path does not exist.";
                break;
            case LoadAndStoreStrategy::Error::CouldNotReadCamInfo:
                message = "Could not read the camera info file info.json.";
                break;
            case LoadAndStoreStrategy::Error::CamInfoPathIsNotAJSONFile:
                message = "The camera info file info.json is not a valid file.";
                break;
            case LoadAndStoreStrategy::Error::NoImagesFound:
                message = "No images found at the specified images path.";
                break;
            case LoadAndStoreStrategy::Error::ObjectModelsPathDoesNotExist:
                message = "The object models path does not exist.";
                break;
            case LoadAndStoreStrategy::Error::ObjectModelsPathIsNotAFolder:
                message = "The object models path is not a folder.";
                break;
            case LoadAndStoreStrategy::Error::PosesPathDoesNotExist:
                message = "The specified poses file does not exist.";
                break;
            case LoadAndStoreStrategy::Error::FailedToPersistPosePosesFileCouldNotBeRead:
                message = "Could not read the poses file (error while trying to save poses).";
                break;
            case LoadAndStoreStrategy::Error::FailedToPersistPosePosesPathIsNotAFile:
                message = "The specified poses file is not a file (error while trying to save poses).";
                break;
            case LoadAndStoreStrategy::Error::PosesPathIsNotReadable:
                message = "The specified poses file is not readable.";
                break;
            case LoadAndStoreStrategy::Error::InvalidCameraMatrices:
                message = "There were invalid camera matrices in the cam.info file.";
                break;
            case LoadAndStoreStrategy::Error::PosesWithInvalidPosesData:
                message = "There were invalid entries for poses in the poses file.";
                break;
            case LoadAndStoreStrategy::Error::NotEnoughSegmentationImages:
                message = "There were not enough segmentation images to match them with all images.";
                break;
        }
        displayWarning("An error occured in the data manger", message);
    }
}

const QString MainWindow::SETTINGS_NAME = "FlorettiKonfetti Inc.";
const QString MainWindow::SETTINGS_PROGRAM_NAME = "6D-PAT";
const QString MainWindow::SETTINGS_GROUP_NAME = "mainwindow";
const QString MainWindow::WINDOW_IS_FULLSCREEN_KEY = "isfullscreen";
const QString MainWindow::WINDOW_SIZE_KEY = "windowsize";
const QString MainWindow::WINDOW_POSITION_KEY = "windowposition";
const QString MainWindow::SPLITTER_MAIN_SIZE_LEFT_KEY = "splitterMainLeftSize";
const QString MainWindow::SPLITTER_MAIN_SIZE_RIGHT_KEY = "splitterMainRightSize";
const QString MainWindow::SPLITTER_LEFT_SIZE_TOP_KEY = "splitterLeftLeftSize";
const QString MainWindow::SPLITTER_LEFT_SIZE_BOTTOM_KEY = "splitterLeftRightSize";
const QString MainWindow::SPLITTER_RIGHT_SIZE_TOP_KEY = "splitterRightLeftSize";
const QString MainWindow::SPLITTER_RIGHT_SIZE_BOTTOM_KEY = "splitterRightRightSize";
