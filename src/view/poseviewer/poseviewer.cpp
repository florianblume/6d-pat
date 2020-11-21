#include "poseviewer.hpp"
#include "ui_poseviewer.h"
#include "view/misc/displayhelper.hpp"

#include <QRect>

PoseViewer::PoseViewer(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::PoseViewer),
        poseViewer3DWidget(new PoseViewer3DWidget) {
    ui->setupUi(this);

    poseViewer3DWidget->setParent(ui->graphicsContainer);

    DisplayHelper::setIcon(ui->buttonSwitchView, fa::toggleoff, 18);
    ui->buttonSwitchView->setToolTip("Click to switch views between segmentation \n"
                                     "image (if available) and normal image.");
    ui->buttonSwitchView->setEnabled(false);

    DisplayHelper::setIcon(ui->buttonResetPosition, fa::arrows, 18);
    ui->buttonResetPosition->setToolTip("Click to reset the position of the image.");
    ui->buttonResetPosition->setEnabled(false);

    // Those two buttons are not actual buttons but show helper icons
    // They are buttons and not labels so that they can get activated and deactivated
    // together with the respective sliders
    DisplayHelper::setIcon(ui->buttonTransparency, fa::cube, 18);
    DisplayHelper::setIcon(ui->buttonZoom, fa::search, 18);

    connect(poseViewer3DWidget, &PoseViewer3DWidget::positionClicked,
            this, &PoseViewer::onImageClicked);
    connect(poseViewer3DWidget, &PoseViewer3DWidget::poseSelected,
            this, &PoseViewer::poseSelected);
}

PoseViewer::~PoseViewer() {
    delete ui;
}

void PoseViewer::setPoseRecoverer(PoseRecoveringController *value) {
    Q_ASSERT(value);
    if (!this->poseRecoverer.isNull()) {
        disconnect(poseRecoverer, &PoseRecoveringController::correspondencesChanged,
                   this, &PoseViewer::onCorrespondencesChanged);
        disconnect(poseRecoverer, &PoseRecoveringController::stateChanged,
                   this, &PoseViewer::poseRecovererStateChanged);
    }
    poseRecoverer = value;
    connect(poseRecoverer, &PoseRecoveringController::correspondencesChanged,
            this, &PoseViewer::onCorrespondencesChanged);
    connect(poseRecoverer, &PoseRecoveringController::stateChanged,
            this, &PoseViewer::poseRecovererStateChanged);
}

void PoseViewer::setSettingsStore(SettingsStore *settingsStore) {
    Q_ASSERT(settingsStore);
    if (!this->settingsStore.isNull()) {
        disconnect(settingsStore, &SettingsStore::settingsChanged,
                   this, &PoseViewer::currentSettingsChanged);
    }
    this->settingsStore = settingsStore;
    connect(settingsStore, &SettingsStore::settingsChanged,
            this, &PoseViewer::currentSettingsChanged);
    poseViewer3DWidget->setSettings(settingsStore->currentSettings());
}

void PoseViewer::setPoses(const QList<PosePtr> &poses) {
    // TODO
    setSliderTransparencyEnabled(poses.size() > 0);
}

void PoseViewer::setModelManager(ModelManager *value) {
    Q_ASSERT(value);
    if (!modelManager.isNull()) {
        disconnect(modelManager, &ModelManager::poseAdded,
                   this, &PoseViewer::onPoseAdded);
        disconnect(modelManager, &ModelManager::poseDeleted,
                   this, &PoseViewer::onPoseDeleted);
        disconnect(modelManager, &ModelManager::dataChanged, this, &PoseViewer::onDataChanged);
    }
    modelManager = value;
    connect(modelManager, &ModelManager::poseAdded,
            this, &PoseViewer::onPoseAdded);
    connect(modelManager, &ModelManager::poseDeleted,
            this, &PoseViewer::onPoseDeleted);
    connect(modelManager, &ModelManager::dataChanged, this, &PoseViewer::onDataChanged);
}

void PoseViewer::connectModelManagerSlots() {
}

void PoseViewer::setSliderZoomEnabled(bool enabled) {
    ui->sliderZoom->setEnabled(enabled);
    ui->buttonZoom->setEnabled(enabled);
}

void PoseViewer::setSliderTransparencyEnabled(bool enabled) {
    ui->sliderTransparency->setEnabled(enabled);
    ui->buttonTransparency->setEnabled(enabled);
}

ImagePtr PoseViewer::currentlyViewedImage() {
    return currentlyDisplayedImage;
}

void PoseViewer::setImage(ImagePtr image) {
    currentlyDisplayedImage = image;

    poseViewer3DWidget->removePoses();
    poseViewer3DWidget->setClicks({});
    ui->buttonResetPosition->setEnabled(true);
    ui->sliderTransparency->setValue(100);
    setSliderZoomEnabled(true);

    qDebug() << "Displaying image (" + currentlyDisplayedImage->imagePath() + ").";

    // Enable/disable functionality to show only segmentation image instead of normal image
    if (currentlyDisplayedImage->segmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        // If we don't find a segmentation image, set that we will now display the normal image
        // because the formerly set image could have had a segmentation image and set this value
        // to false
        showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    QString toDisplay = showingNormalImage ?  currentlyDisplayedImage->absoluteImagePath() :
                                    currentlyDisplayedImage->absoluteSegmentationImagePath();
    QList<PosePtr> posesForImage = modelManager->posesForImage(*image);
    poseViewer3DWidget->setBackgroundImageAndPoses(toDisplay, image->getCameraMatrix(), posesForImage);
    setSliderTransparencyEnabled(posesForImage.size() > 0);
}

void PoseViewer::onSelectedImageChanged(int index) {
    QList<ImagePtr> images = modelManager->images();
    Q_ASSERT_X(index >= 0 && index < images.size(), "onSelectedImageChanged", "Index out of bounds.");
    ImagePtr image = images[index];
    setImage(image);
}

void PoseViewer::reset() {
    qDebug() << "Resetting pose viewer.";
    poseViewer3DWidget->reset();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
    setSliderTransparencyEnabled(false);
    setSliderZoomEnabled(false);
    currentlyDisplayedImage.reset();
}

void PoseViewer::reloadPoses() {
    if (!currentlyDisplayedImage.isNull()) {
        setImage(currentlyDisplayedImage);
    } else {
        reset();
    }
}

void PoseViewer::onPoseCreationAborted() {
    poseViewer3DWidget->setClicks({});
}

void PoseViewer::onCorrespondencesChanged() {
    poseViewer3DWidget->setClicks(poseRecoverer->points2D());
}

void PoseViewer::poseRecovererStateChanged(PoseRecoveringController::State /*state*/) {
    poseViewer3DWidget->setClicks(poseRecoverer->points2D());
}

void PoseViewer::currentSettingsChanged(SettingsPtr settings) {
    // Cannot accept empty settings but should never happen
    Q_ASSERT(!settings.isNull());
    poseViewer3DWidget->setSettings(settings);
}

void PoseViewer::selectPose(PosePtr selected, PosePtr deselected) {
    poseViewer3DWidget->selectPose(selected, deselected);
}

void PoseViewer::switchImage() {
    showingNormalImage = !showingNormalImage;

    DisplayHelper::setIcon(ui->buttonSwitchView,
                           (showingNormalImage ? fa::toggleon : fa::toggleoff),
                           18);

    if (showingNormalImage)
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->absoluteImagePath(),
                                               currentlyDisplayedImage->getCameraMatrix());
    else
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->absoluteSegmentationImagePath(),
                                               currentlyDisplayedImage->getCameraMatrix());

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void PoseViewer::onOpacityChanged(int opacity) {
    poseViewer3DWidget->setObjectsOpacity(opacity / 100.0);
}

void PoseViewer::onZoomChanged(int zoom) {
    this->zoom = zoom;
    if (zoom == 1) {
        this->zoomMultiplier = 0.5f;
    } else if (zoom == 2) {
        this->zoomMultiplier = 1.f;
    } else if (zoom == 3) {
        this->zoomMultiplier = 2.f;
    }
    poseViewer3DWidget->setGeometry(QRect(poseViewer3DWidget->x(),
                                          poseViewer3DWidget->y(),
                                          poseViewer3DWidget->imageSize().width() * this->zoomMultiplier,
                                          poseViewer3DWidget->imageSize().height() * this->zoomMultiplier));
}

void PoseViewer::resetPositionOfGraphicsView() {
    poseViewer3DWidget->setGeometry(0, 0, poseViewer3DWidget->width(), poseViewer3DWidget->height());
}

void PoseViewer::onImageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->imagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    poseRecoverer->add2DPoint(point);
}

void PoseViewer::onPoseDeleted(PosePtr pose) {
    poseViewer3DWidget->removePose(pose->id());
    poseViewer3DWidget->setClicks({});
}

void PoseViewer::onPoseAdded(PosePtr pose) {
    poseViewer3DWidget->addPose(pose);
    setSliderTransparencyEnabled(true);
    poseViewer3DWidget->setClicks({});
}

void PoseViewer::onDataChanged(int data) {
    // If only poses changed we only reload those
    if (data == Data::Poses) {
        reloadPoses();
    } else {
        reset();
    }
}
