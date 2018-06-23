#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "view/settings/settingsdialog.hpp"
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

    // If the selected image changes, we also need to cancel any started creation of a correspondence
    connect(ui->galleryLeft, SIGNAL(selectedItemChanged(int)),
            this, SIGNAL(correspondenceCreationAborted()));
    connect(ui->galleryRight, SIGNAL(selectedItemChanged(int)),
            this, SIGNAL(correspondenceCreationAborted()));
    connect(ui->navigationLeft, SIGNAL(pathChanged(QString)),
            this, SIGNAL(imagesPathChanged(QString)));
    connect(ui->navigationRight, SIGNAL(pathChanged(QString)),
            this, SIGNAL(objectModelsPathChanged(QString)));
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
    ui->correspondenceViewer->setModelManager(modelManager);
    ui->correspondenceEditor->setModelManager(modelManager);
}

void MainWindow::resetCorrespondenceViewer() {
    ui->correspondenceViewer->reset();
}

void MainWindow::setPreferencesStore(PreferencesStore *preferencesStore) {
    if (this->preferencesStore) {
        disconnect(this->preferencesStore, SIGNAL(preferencesChanged(QString)),
                   this, SLOT(onPreferencesChanged(QString)));
    }
    this->preferencesStore = preferencesStore;
    connect(preferencesStore, SIGNAL(preferencesChanged(QString)),
            this, SLOT(onPreferencesChanged(QString)));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (correspondenceCreationInProgress) {
        // Reset correspondence creation because the user clicked anywhere
        onCorrespondenceCreationReset();
        emit correspondenceCreationAborted();
    }
}

void MainWindow::setStatusBarText(const QString& text) {
    statusBarLabel->setText(text);
}
//! Mouse handling, i.e. clicking in the lower left widget and dragging a line to the lower right widget
void MainWindow::onImageClicked(Image* image, QPoint position) {
    //! No need to check for whether the right widget was clicked because the only time this method
    //! will be called is when the object image picker received a click on the image
    if (ui->correspondenceEditor->isDisplayingObjectModel()) {
        if (correspondenceCreationInProgress) {
            displayWarning("Correspondence creation", "You need to click"
                                                      " the object model"
                                                      " to add the 2D-3D"
                                                      " correspondence.");
        } else {
            QGuiApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
            correspondenceCreationInProgress = true;
            emit imageClicked(image, position);
        }
    } else {
        QMessageBox::warning(this, "Select object model first", "Please select an object model from the list\n"
                                                                "of object models first before trying to create\n"
                                                                "a new correspondence.");
    }
}

void MainWindow::onObjectModelClicked(ObjectModel* objectModel, QVector3D position) {
    if (!correspondenceCreationInProgress) {
        displayWarning("Correspondence creation", "You need to click a position on"
                                                  " the image first before selecting"
                                                  " the corresponding 3D position.");
    }
    QGuiApplication::restoreOverrideCursor();
    correspondenceCreationInProgress = false;
    // The user might click on the overlay before he starts to create a correspondence,
    // i.e. the overlay is not visible yet and not created. But as soon as the user clicks
    // the image and then the object, this adds a correspondence point and the overlay
    // should be hidden.
    emit objectModelClicked(objectModel, position);
}

void MainWindow::onSelectedObjectModelChanged(int index) {
    QList<ObjectModel> objectModels = modelManager->getObjectModels();
    Q_ASSERT(index >= 0 && index < objectModels.size());
    // Ok as long as the addressees are in the same thread and directly process the event.
    ObjectModel *model = new ObjectModel(objectModels.at(index));
    emit selectedObjectModelChanged(model);
    delete model;
    onCorrespondenceCreationReset();
}

void MainWindow::onSelectedImageChanged(int index) {
    QList<Image> images = modelManager->getImages();
    Q_ASSERT(index >= 0 && index < images.size());
    // Ok as long as the addressees are in the same thread and directly process the event.
    Image *image = new Image(images.at(index));
    emit selectedImageChanged(image);
    delete image;
    onCorrespondenceCreationReset();
}

void MainWindow::onImagesPathChangedByNavigation(const QString &path) {
    UniquePointer<Preferences> preferences = preferencesStore->loadPreferencesByIdentifier("default");
    preferences->setImagesPath(path);
    preferencesStore->savePreferences(preferences.get());
}

void MainWindow::onObjectModelsPathChangedByNavigation(const QString &path) {
    UniquePointer<Preferences> preferences = preferencesStore->loadPreferencesByIdentifier("default");
    preferences->setObjectModelsPath(path);
    preferencesStore->savePreferences(preferences.get());
}

void MainWindow::displayWarning(const QString &title, const QString &text) {
    QMessageBox::warning(this, title, text);
}

void MainWindow::onCorrespondencePointStarted(QPoint point2D, int currentNumberOfPoints, int requiredNumberOfPoints) {
    setStatusBarText("Please select the corresponding 3D point [" +
                                QString::number(currentNumberOfPoints)
                                + " of min. " +
                                QString::number(requiredNumberOfPoints)
                     + "].");
    emit correspondencePointStarted(point2D, currentNumberOfPoints, requiredNumberOfPoints);
}

void MainWindow::onCorrespondencePointFinished(QVector3D point3D, int currentNumberOfPoints, int requiredNumberOfPoints) {
    setStatusBarText("Please select another correspondence point [" +
                                QString::number(currentNumberOfPoints)
                                + " of min. " +
                                QString::number(requiredNumberOfPoints)
                     + "].");
    emit correspondencePointFinished(point3D, currentNumberOfPoints, requiredNumberOfPoints);
}

void MainWindow::onCorrespondenceCreated() {
    onCorrespondenceCreationReset();
}

void MainWindow::onCorrespondenceCreationReset() {
    setStatusBarText("Ready.");
    QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    correspondenceCreationInProgress = false;
}

void MainWindow::onCorrespondenceCreationRequested() {
    setStatusBarText("Creating correspondence...");
    emit requestCorrespondenceCreation();
    QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::onPreferencesChanged(const QString &identifier) {
    UniquePointer<Preferences> preferences = preferencesStore->loadPreferencesByIdentifier(identifier);
    setPathOnLeftBreadcrumbView(preferences->getImagesPath());
    setPathOnRightBreadcrumbView(preferences->getObjectModelsPath());
    onCorrespondenceCreationReset();
}

void MainWindow::onActionAboutTriggered()
{
    QMessageBox::about(this, tr("About Otiat"), tr("Object to image annotator, "
                                                   "or shorter Otiat, is a tool "
                                                   "that allows users to drag and "
                                                   "position 3D models of objects "
                                                   "on images. Depending on the "
                                                   "selected storage options the "
                                                   "position and rotation are "
                                                   "persisted for later use. "
                                                   "The so annotated images can "
                                                   "be used to e.g. train a neural "
                                                   "network."));
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
    emit correspondenceCreationAborted();
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
