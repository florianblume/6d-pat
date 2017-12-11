#include "maincontroller.hpp"
#include "view/gallery/galleryimagemodel.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSettings>
#include <QDir>
#include <iostream>

//! empty initialization of strategy so that we can set the path later and do so
//! in a background thread to keep application reactive
MainController::MainController(int &argc, char *argv[]) :
    QApplication(argc, argv),
    strategy(),
    modelManager(strategy) {
}

MainController::~MainController() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("maincontroller");
    settings.setValue("imagesPath", strategy.getImagesPath().path());
    settings.setValue("objectModelsPath", strategy.getObjectModelsPath().path());
    settings.setValue("correspondencesPath", strategy.getCorrespondencesPath().path());
    settings.endGroup();

    //! Persist the object color codes so that the user does not have to enter them at each program start
    //! But first remove all old entries, in case that the user deleted some codes
    settings.beginGroup("maincontroller-settings");
    settings.remove("");
    for (uint i = 0; i < modelManager.getObjectModelsSize(); i++) {
        const ObjectModel* model = modelManager.getObjectModel(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        settings.setValue(model->getAbsolutePath(),
                                     currentSettingsItem->getSegmentationCodeForObjectModel(model));
    }
    settings.endGroup();

    delete currentSettingsItem;
    delete galleryImageModel;
    delete galleryObjectModelModel;
}

void MainController::initialize() {
    // TODO: initalization in background thread, only disable UI until loading has finished
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("maincontroller");
    strategy.setImagesPath(settings.value("imagesPath", QDir::homePath()).toString());
    strategy.setObjectModelsPath(settings.value("objectModelsPath", QDir::homePath()).toString());
    strategy.setCorrespondencesPath(settings.value("correspondencesPath", QDir::homePath()).toString());
    settings.endGroup();

    initializeSettingsItem();
    initializeMainWindow();
}

void MainController::initializeSettingsItem() {
    currentSettingsItem = new SettingsItem("default", &modelManager);
    currentSettingsItem->setImagesPath(strategy.getImagesPath().path());
    currentSettingsItem->setObjectModelsPath(strategy.getObjectModelsPath().path());
    currentSettingsItem->setCorrespondencesPath(strategy.getCorrespondencesPath().path());
    currentSettingsItem->setSegmentationImageFilesSuffix(strategy.getSegmentationImageFilesSuffix());
    currentSettingsItem->setImageFilesExtension(strategy.getImageFilesExtension());

    //! Read persisted object color codes
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");
    settings.beginGroup("maincontroller-settings");
    for (uint i = 0; i < modelManager.getObjectModelsSize(); i++) {
        const ObjectModel* model = modelManager.getObjectModel(i);
        const QString objectModelIdentifier = model->getAbsolutePath();
        QString storedCode = settings.value(objectModelIdentifier, "").toString();
        if (storedCode.compare("") != 0)
            currentSettingsItem->setSegmentationCodeForObjectModel(model, storedCode);
    }
    settings.endGroup();
}
void MainController::initializeMainWindow() {
    mainWindow.setSettingsItem(currentSettingsItem);
    mainWindow.setSettingsDialogDelegate(this);

    //! The reason why the breadcrumbs receive an object of the path type of the boost filesystem library
    //! is because the breadcrumb views have to split the path to show it.
    mainWindow.setPathOnLeftBreadcrumbView(strategy.getImagesPath().path());
    mainWindow.setPathOnRightBreadcrumbView(strategy.getObjectModelsPath().path());
    mainWindow.setPathOnLeftNavigationControls(QString(strategy.getImagesPath().path()));
    mainWindow.setPathOnRightNavigationControls(QString(strategy.getObjectModelsPath().path()));
    //! left navigation selects the images folder
    mainWindow.addListenerToLeftNavigationControls([this] (QString& newPath) {this->strategy.setImagesPath(newPath);
                                                                             this->currentSettingsItem->setImagesPath(newPath);});
    //! right navigation selects the object models folder
    mainWindow.addListenerToRightNavigationControls([this] (QString& newPath) {this->strategy.setObjectModelsPath(newPath);
                                                                              this->currentSettingsItem->setObjectModelsPath(newPath);});

    //! The models do not need to notify the gallery of any changes on the data because the list view
    //! has its own update loop, i.e. automatically fetches new data
    galleryImageModel = new GalleryImageModel(&modelManager);
    mainWindow.setGalleryImageModel(galleryImageModel);
    galleryObjectModelModel = new GalleryObjectModelModel(&modelManager);
    setSegmentationCodesOnGalleryObjectModelModel();
    mainWindow.setGalleryObjectModelModel(galleryObjectModelModel);
    mainWindow.setModelManager(&modelManager);

    mainWindow.setStatusBarText("Ready");

    //! Connect the main window's reactions to the user clicking on a displayed image or on an object
    //! model to delegate any further computation to this controller
    connect(&mainWindow, SIGNAL(imageClicked(const Image*,QPointF)),
            this, SLOT(onImageClicked(const Image*,QPointF)));
    connect(&mainWindow, SIGNAL(objectModelClickedAt(const ObjectModel*,QVector3D)),
            this, SLOT(onObjectModelClickedAt(const ObjectModel*,QVector3D)));
    connect(&mainWindow, SIGNAL(correspondenceCreationAborted()), this, SLOT(onCorrespondenceCreationAborted()));
}

void MainController::setSegmentationCodesOnGalleryObjectModelModel() {
    QMap<const ObjectModel*, QString> codes;
    currentSettingsItem->getSegmentationCodes(codes);
    galleryObjectModelModel->setSegmentationCodesForObjectModels(codes);
}

void MainController::onImageClicked(const Image* image, QPointF position) {
    if (lastClickedImage != image)
        correspondingPoints.clear();

    lastClickedImage = image;
    lastClickedImagePosition = position;
    // TODO: show how many points are missing until an actual correspondence can be created
    mainWindow.setStatusBarText("Please select the corresponding 3D point [" +
                            QString::number(correspondingPoints.size() + 1)
                            + " of 4].");
}

void MainController::onObjectModelClickedAt(const ObjectModel* objectModel, QVector3D position) {
    //! If we can't find a previously clicked position on the image just return, the user has to select a 2D
    //! point on the image first
    if (!lastClickedImage)
        return;

    correspondingPoints.append(CorrespondingPoints{lastClickedImagePosition, objectModel, position});

    if (correspondingPoints.size() < 3) {
        mainWindow.setStatusBarText("Please select another correspondence [" +
                                QString::number(correspondingPoints.size() + 1)
                                + " of 4].");
    } else if (correspondingPoints.size() == 3) {
        mainWindow.setStatusBarText("Please select the last correspondence.");
    } else {
        mainWindow.setStatusBarText("Creating correspondence, please wait.");
        std::vector<cv::Point3d> objectPoints;
        std::vector<cv::Point2d> imagePoints;
        for (CorrespondingPoints &point : correspondingPoints) {
            objectPoints.push_back(cv::Point3d(point.pointIn3D.x(), point.pointIn3D.y(), point.pointIn3D.z()));
            imagePoints.push_back(cv::Point2d(point.pointIn2D.x(), point.pointIn2D.y()));
        }

        QImage loadedImage = QImage(lastClickedImage->getAbsoluteImagePath());
        double focalLength = loadedImage.size().width(); // Approximate focal length.
        cv::Point2d center = cv::Point2d(loadedImage.size().width()/2,loadedImage.size().height()/2);
        cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << focalLength, 0, center.x, 0 , focalLength, center.y, 0, 0, 1);
        cv::Mat coefficient = cv::Mat::zeros(4,1,cv::DataType<double>::type);

        cv::Mat resultRotation;
        cv::Mat resultTranslation;

        cv::solvePnP(objectPoints, imagePoints, cameraMatrix, coefficient, resultRotation, resultTranslation);

        // TODO: use translation values
        ObjectImageCorrespondence correspondence = ObjectImageCorrespondence("", 0, 0, 0,
                                  resultRotation.at<float>(0, 0),
                                  resultRotation.at<float>(0, 1),
                                  resultRotation.at<float>(0, 2),
                                  0,
                                  lastClickedImage,
                                  objectModel);
        modelManager.addObjectImageCorrespondence(correspondence);

        correspondingPoints.clear();
        lastClickedImage = Q_NULLPTR;
        mainWindow.setStatusBarText("Ready.");
    }
}


void MainController::onCorrespondenceCreationAborted() {
    mainWindow.setStatusBarText("Ready");
    lastClickedImage = Q_NULLPTR;
    correspondingPoints.clear();
}

void MainController::showView() {
    mainWindow.show();
}

void MainController::applySettings(const SettingsItem* settingsItem) {
    //! Save the settings item to be set
    delete currentSettingsItem;
    this->currentSettingsItem = new SettingsItem(*settingsItem);

    //! Set the values
    strategy.setImagesPath(settingsItem->getImagesPath());
    strategy.setObjectModelsPath(settingsItem->getObjectModelsPath());
    strategy.setCorrespondencesPath(settingsItem->getCorrespondencesPath());
    strategy.setImageFilesExtension(settingsItem->getImageFilesExtension());
    strategy.setSegmentationImageFilesSuffix(settingsItem->getSegmentationImageFilesSuffix());

    //! Update view accordingly
    mainWindow.setPathOnLeftBreadcrumbView(settingsItem->getImagesPath());
    mainWindow.setPathOnLeftNavigationControls(settingsItem->getImagesPath());
    mainWindow.setPathOnRightBreadcrumbView(settingsItem->getObjectModelsPath());
    mainWindow.setPathOnRightNavigationControls(settingsItem->getObjectModelsPath());
    setSegmentationCodesOnGalleryObjectModelModel();

    //! And update the settings item on the main window for the settings dialog, etc.
    mainWindow.setSettingsItem(currentSettingsItem);
}
