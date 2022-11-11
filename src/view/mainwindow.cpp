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
#include <QStandardPaths>
#include <QFileDialog>
#include <QCheckBox>

//! The main window of the application that holds the individual components.<
MainWindow::MainWindow(QWidget *parent,
                       ModelManager *modelManager,
                       SettingsStore *settingsStore) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_settingsStore(settingsStore),
    m_modelManager(modelManager) {

    ui->setupUi(this);

    m_progressDialog = new QProgressDialog;
    m_progressDialog->setRange(0, 0);
    m_progressDialog->setCancelButton(Q_NULLPTR);
    m_progressDialog->setModal(true);
    m_progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    m_progressDialog->setFixedSize(m_progressDialog->size());

    //tutorialScreen = new TutorialScreen(this);
    //tutorialScreen->setGeometry(this->geometry());

    statusBar()->addPermanentWidget(m_statusBarLabel, 1);
    setStatusBarText(QString("Loading..."));
    connect(modelManager, &ModelManager::stateChanged,
            this, &MainWindow::onModelManagerStateChanged);

    connect(settingsStore, &SettingsStore::currentSettingsChanged,
            this, &MainWindow::onSettingsChanged);

    m_galleryImageModel = new GalleryImageModel(modelManager);
    setGalleryImageModel(m_galleryImageModel);
    m_galleryObjectModelModel = new GalleryObjectModelModel(modelManager);
    setGalleryObjectModelModel(m_galleryObjectModelModel);

    setPathsOnGalleriesAndBreadcrumbs();

    connect(ui->breadcrumbLeft, &BreadcrumbView::selectedPathChanged,
            this, &MainWindow::onImagesPathChangedByNavigation);
    connect(ui->breadcrumbRight, &BreadcrumbView::selectedPathChanged,
            this, &MainWindow::onObjectModelsPathChangedByNavigation);

    // If the selected image changes, we also need to cancel any started creation of a pose
    connect(ui->galleryLeft, &Gallery::selectedItemChanged,
            this, &MainWindow::abortPoseCreationRequested);
    connect(ui->galleryRight, &Gallery::selectedItemChanged,
            this, &MainWindow::abortPoseCreationRequested);

    connect(ui->poseViewer, &PoseViewer::snapshotSaved,
            this, &MainWindow::onSnapshotSaved);

    setStatusBarText("Ready.");

    m_progressDialog->close();
}

MainWindow::~MainWindow() {
    m_progressDialog->close();
    delete m_progressDialog;
    delete m_galleryImageModel;
    delete m_galleryObjectModelModel;
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

void MainWindow::setStatusBarTextIdle() {
    bool selectItemsFirst = false;
    QString message;
    if (!ui->galleryLeft->hasItemSelection()) {
        message += "Select an image from the images gallery. ";
        selectItemsFirst = true;
    }
    if (!ui->galleryRight->hasItemSelection()) {
        message += "Select an object model from the object models gallery.";
        selectItemsFirst = true;
    }
    if (selectItemsFirst) {
        setStatusBarText(message);
    } else {
        Qt::MouseButton buttonCorrespondences = this->m_settingsStore->currentSettings()->addCorrespondencePointMouseButton();
        QString buttonCorrespondencesString = DisplayHelper::qtMouseButtonToString(buttonCorrespondences);
        Qt::MouseButton buttonSelectPoseRenderable = this->m_settingsStore->currentSettings()->selectPoseRenderableMouseButton();
        QString buttonSelectPoseRenderableString = DisplayHelper::qtMouseButtonToString(buttonSelectPoseRenderable);
        Qt::MouseButton buttonMoveImage = this->m_settingsStore->currentSettings()->moveBackgroundImageRenderableMouseButton();
        QString buttonMoveImageString = DisplayHelper::qtMouseButtonToString(buttonMoveImage);
        setStatusBarText(
                    "Start adding correspondences with [" + buttonCorrespondencesString +
                    "] on image or object model. Select a pose with [" + buttonSelectPoseRenderableString + "]. " +
                    "Move the image with [" + buttonMoveImageString + "].");
    }
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
    if (!m_showInitialized) {
        readSettings();
    }
    m_showInitialized = true;
    QMainWindow::showEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    QMainWindow::resizeEvent(e);
    if (m_tutorialScreen) {
        m_tutorialScreen->setGeometry(0, 0,
                                    this->geometry().width(), this->geometry().height());
        m_tutorialScreen->setCurrentViewWindow(QRect(100, 100, 100, 100));
    }
}

void MainWindow::setStatusBarText(const QString& text) {
    m_statusBarLabel->setText(tr(text.toStdString().c_str()));
}

void MainWindow::showProgressView(bool show) {
    if (show) {
        m_progressDialog->show();
        this->setGraphicsEffect(new QGraphicsBlurEffect);
    } else {
        m_progressDialog->close();
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
    m_progressDialog->setLabelText(tr("Loading data..."));
    showProgressView(show);
}

void MainWindow::showPoseRecoveringProgressView(bool show) {
    m_progressDialog->setLabelText(tr("Recovering pose..."));
    showProgressView(show);
}

void MainWindow::clearGallerySelections() {
    ui->galleryLeft->clearSelection(false);
    ui->galleryRight->clearSelection(false);
}

void MainWindow::onImagesPathChangedByNavigation(const QString &path) {
    SettingsPtr settings = m_settingsStore->currentSettings();
    settings->setImagesPath(path);
    m_settingsStore->saveCurrentSettings();
}

void MainWindow::onObjectModelsPathChangedByNavigation(const QString &path) {
    SettingsPtr settings = m_settingsStore->currentSettings();
    settings->setObjectModelsPath(path);
    m_settingsStore->saveCurrentSettings();
}

void MainWindow::displayWarning(const QString &title, const QString &text) {
    DisplayHelper::QMessageBoxPtr messageBox = DisplayHelper::messageBox(this,
                                                                         QMessageBox::Warning,
                                                                         tr(title.toStdString().c_str()),
                                                                         tr(text.toStdString().c_str()),
                                                                         "Ok", QMessageBox::AcceptRole);
    messageBox->exec();
}

bool MainWindow::showSaveUnsavedChangesDialog() {
    DisplayHelper::QMessageBoxPtr messageBox = DisplayHelper::messageBox(this,
                                                                         QMessageBox::Warning,
                                                                         "Unsaved Edits",
                                                                         "You have unsaved edits of the current poses."
                                                                         "\n\n"
                                                                         "Save them now?",
                                                                         "Yes", QMessageBox::YesRole,
                                                                         "No", QMessageBox::NoRole);
    auto returnValue = messageBox->exec();
    // Message box returns 0 when yes is pressed
    return 0 == returnValue;
}

void MainWindow::setPathsOnGalleriesAndBreadcrumbs() {
    SettingsPtr settings = m_settingsStore->currentSettings();
    m_galleryObjectModelModel->setSegmentationCodesForObjectModels(settings->segmentationCodes());
    ui->breadcrumbLeft->setCurrentPath(settings->imagesPath());
    ui->breadcrumbRight->setCurrentPath(settings->objectModelsPath());
}

void MainWindow::onSettingsChanged(SettingsPtr settings) {
    m_galleryObjectModelModel->setSegmentationCodesForObjectModels(settings->segmentationCodes());
    setPathsOnGalleriesAndBreadcrumbs();
    if (m_statusBarLabel->text().contains("Start adding correspondences")) {
        // A bit hacky but this way we know when to update the displayed buttons
        setStatusBarTextIdle();
    }
}

void MainWindow::onActionAboutTriggered() {
    QMessageBox::about(this, "6D-PAT",
                       tr("6D pose annotation tool, "
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
                            "https://github.com/florianblume/6d-pat\n"
                            "\n"
                            "Created by Florian Blume, 2020 - 2021, GPLv3"));
}

void MainWindow::onActionExitTriggered() {
    handleClosingProgram();
    QApplication::quit();
}

void MainWindow::onActionSettingsTriggered() {
    SettingsDialog* settingsDialog = new SettingsDialog(this);
    settingsDialog->setSettingsStoreAndObjectModels(m_settingsStore,
                                                    m_modelManager->objectModels());
    settingsDialog->show();
}

void MainWindow::onActionAbortCreationTriggered() {
    setStatusBarText(tr("Creating pose..."));
    Q_EMIT abortPoseCreationRequested();
}

void MainWindow::onActionResetTriggered() {
    setStatusBarTextIdle();
    Q_EMIT resetRequested();
}

void MainWindow::onActionReloadViewsTriggered() {
    Q_EMIT reloadViewsRequested();
    setStatusBarTextIdle();
}

void MainWindow::onActionTakeSnapshotTriggered() {
    if (!ui->poseViewer->currentlyViewedImage().isNull()) {
        QSettings settings("Floretti Konfetti Inc.", "6D-PAT");
        QString picturesLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        Image *currentlyViewedImage = ui->poseViewer->currentlyViewedImage().get();
        const QString &imagePath = currentlyViewedImage->absoluteImagePath();
        const QString &imageName = QFileInfo{imagePath}.fileName();
        // Default new image name and path when none have been set
        QString defaultPath = QFileInfo(QDir(picturesLocation).filePath("snapshot_" + imageName)).absoluteFilePath();
        QString path = settings.value("snapshotPath", defaultPath).toString();
        QString chosenPath = QFileDialog::getSaveFileName(this,
                                                          tr("Save snapshot as"),
                                                          path,
                                                          tr("PNG Files (*.png)"),
                                                          Q_NULLPTR,
                                                          QFileDialog::DontUseNativeDialog);
        QFileInfo chosenPathInfo(chosenPath);
        bool takeSnapshot = true;
        if (chosenPathInfo.exists()) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "File exists", "The specified file exists. Overwrite?",
                                                                      QMessageBox::Yes|QMessageBox::No);
            takeSnapshot = reply == QMessageBox::Yes;
        }
        if (chosenPath != "") {
            settings.setValue("snapshotPath", chosenPath);
            ui->poseViewer->takeSnapshot(chosenPath);
        }
    } else {
        displayWarning("Error taking snapshot", "Please select an image before taking a snapshot.");
    }
}

void MainWindow::onSnapshotSaved() {
    QSettings settings("Floretti Konfetti Inc.", "6D-PAT");
    bool showSnapshotSavedMessageBox = settings.value("showSnapshotSavedMessageBox", true).toBool();
    if (showSnapshotSavedMessageBox) {
        QCheckBox *cb = new QCheckBox("Do not show again");
        QMessageBox msgbox;
        msgbox.setText("Snapshot successfullly saved!");
        msgbox.setIcon(QMessageBox::Icon::Information);
        msgbox.addButton(QMessageBox::Ok);
        msgbox.setCheckBox(cb);

        QObject::connect(cb, &QCheckBox::stateChanged, [](int state){
            QSettings settings("Floretti Konfetti Inc.", "6D-PAT");
            settings.setValue("showSnapshotSavedMessageBox",
                              static_cast<Qt::CheckState>(state) != Qt::CheckState::Checked);
        });

        msgbox.exec();
    }
}

void MainWindow::onActionTutorialScreenTriggered() {
    displayWarning("Tutorial not yet implemented", "Unfortunately, the tutorial has not yet been implemented. "
                                                   "Feel free to contribute one on GitHub: https://github.com/"
                                                   "florianblume/6d-pat/issues/63");
}

void MainWindow::onModelManagerStateChanged(ModelManager::State state,
                                            const QString &error) {
    if (state == ModelManager::Error || !error.isNull()) {
        displayWarning("Error while loading or saving data", error);
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
