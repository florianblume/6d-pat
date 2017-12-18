#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "view/aboutdialog/aboutdialog.hpp"
#include "view/settings/settingsdialog.hpp"
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLayout>

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

//! The main window of the application that holds the individual components.<
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    readSettings();
    statusBar()->addPermanentWidget(statusBarLabel, 1);
    setStatusBarText(QString("Loading..."));

    // Cannot define signal to signal mapping in the designer, thus the connections are defined here
    connect(ui->galleryLeft, SIGNAL(selectedItemChanged(int)),
            this, SIGNAL(selectedItemChanged(int)));
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
    //! We connect the model that displays the 3D model objects to the selected image so that the model
    //! can display the respective models
    connect(ui->galleryLeft, &Gallery::selectedItemChanged, model, &GalleryObjectModelModel::onSelectedImageChanged);
    //! To update the listview of the 3D models we call the reset method when the model receives the signal
    //! that the selection of images on the left has been changed
    connect(model, &GalleryObjectModelModel::displayedObjectModelsChanged, ui->galleryRight, &Gallery::reset);
}

void MainWindow::setModelManager(ModelManager* modelManager) {
    this->modelManager = modelManager;
    ui->correspondenceViewer->setModelManager(modelManager);
}

void MainWindow::resetCorrespondenceEditor() {
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

void MainWindow::setStatusBarText(const QString& text) {
    statusBarLabel->setText(text);
}

void MainWindow::onActionAboutTriggered()
{
    AboutDialog* aboutDialog = new AboutDialog(this);
    aboutDialog->show();
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
    emit correspondenceCreationAborted();
}

//! Mouse handling, i.e. clicking in the lower left widget and dragging a line to the lower right widget
void MainWindow::onImageClicked(Image* image, QPointF position) {
    //! No need to check for whether the right widget was clicked because the only time this method
    //! will be called is when the object image picker received a click on the image
    if (ui->correspondenceEditor->isDisplayingObjectModel()) {
        QGuiApplication::setOverrideCursor(QCursor(Qt::CrossCursor));

        if (clickOverlay == Q_NULLPTR) {
            clickOverlay = new ClickOverlay(this);
            connect(clickOverlay, SIGNAL(clickedAnywhere()), this, SLOT(onOverlayClickedAnywhere()));
        }

        clickOverlay->setGeometry(QRect(this->geometry().x(),
                                        this->geometry().y(),
                                        this->geometry().width(),
                                        this->geometry().height()
                                            - this->statusBar()->geometry().height()));
        clickOverlay->show();
        clickOverlay->raise();
        ui->correspondenceEditor->raise();
        emit imageClicked(image, position);
    } else {
        QMessageBox::warning(this, "Select object model first", "Please select an object model from the list\n"
                                                                "of object models first before trying to create\n"
                                                                "a new correspondence.");
    }
}

void MainWindow::onOverlayClickedAnywhere() {
    QGuiApplication::restoreOverrideCursor();
    clickOverlay->hide();
    emit correspondenceCreationAborted();
}

void MainWindow::onObjectModelClickedAt(ObjectModel* objectModel, QVector3D position) {
    QGuiApplication::restoreOverrideCursor();
    emit objectModelClickedAt(objectModel, position);
}

void MainWindow::onSelectedObjectModelChanged(int index) {
    const QList<ObjectModel> &objectModels = modelManager->getObjectModels();
    Q_ASSERT(index >= 0 && index < objectModels.size());
    emit selectedObjectModelChanged(new ObjectModel(objectModels.at(index)));
}

void MainWindow::onPreferencesChanged(const QString &identifier) {
    UniquePointer<Preferences> preferences = preferencesStore->loadPreferencesByIdentifier(identifier);
    setPathOnLeftBreadcrumbView(preferences->getImagesPath());
    setPathOnRightBreadcrumbView(preferences->getObjectModelsPath());
    ui->correspondenceEditor->reset();
    ui->correspondenceViewer->reset();
}
