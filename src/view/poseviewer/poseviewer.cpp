#include "poseviewer.hpp"
#include "ui_poseviewer.h"
#include "view/misc/displayhelper.hpp"

#include "math.h"

PoseViewer::PoseViewer(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::PoseViewer),
    poseViewer3DWidget(new PoseViewer3DWidget),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager)
{
    ui->setupUi(this);

    poseViewer3DWidget->setParent(ui->graphicsContainer);

    connect(poseViewer3DWidget, &PoseViewer3DWidget::positionClicked,
            this, &PoseViewer::onImageClicked);

    awesome->initFontAwesome();

    ui->buttonSwitchView->setFont(awesome->font(18));
    ui->buttonSwitchView->setIcon(awesome->icon(fa::toggleoff));
    ui->buttonSwitchView->setToolTip("Click to switch views between segmentation \n"
                                     "image (if available) and normal image.");
    ui->buttonSwitchView->setEnabled(false);

    ui->buttonResetPosition->setFont(awesome->font(18));
    ui->buttonResetPosition->setIcon(awesome->icon(fa::arrows));
    ui->buttonResetPosition->setToolTip("Click to reset the position of the image.");
    ui->buttonResetPosition->setEnabled(false);

    if (modelManager) {
        connectModelManagerSlots();
    }

    //connect(ui->openGLWidget, SIGNAL(positionClicked(QPoint)), this, SLOT(onImageClicked(QPoint)));
}

PoseViewer::~PoseViewer() {
    delete ui;
}

void PoseViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    if (this->modelManager) {
        disconnect(modelManager, SIGNAL(posesChanged()), this, SLOT(onPosesChanged()));
        disconnect(modelManager, SIGNAL(poseAdded(QString)),
                   this, SLOT(onPoseAdded(QString)));
        disconnect(modelManager, SIGNAL(poseDeleted(QString)),
                   this, SLOT(onPoseDeleted(QString)));
        disconnect(this->modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        disconnect(this->modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connectModelManagerSlots();
}

Image *PoseViewer::currentlyViewedImage() {
    return currentlyDisplayedImage.data();
}

void PoseViewer::setImage(Image *image) {
    currentlyDisplayedImage.reset(new Image(*image));

    //ui->openGLWidget->removePoses();
    ui->buttonResetPosition->setEnabled(true);

    qDebug() << "Displaying image (" + currentlyDisplayedImage->getImagePath() + ").";

    // Enable/disable functionality to show only segmentation image instead of normal image
    if (currentlyDisplayedImage->getSegmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        // If we don't find a segmentation image, set that we will now display the normal image
        // because the formerly set image could have had a segmentation image and set this value
        // to false
        showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    QString toDisplay = showingNormalImage ?  currentlyDisplayedImage->getAbsoluteImagePath() :
                                    currentlyDisplayedImage->getAbsoluteSegmentationImagePath();
    QList<Pose> posesForImage = modelManager->getPosesForImage(*image);
    poseViewer3DWidget->setBackgroundImageAndPoses(toDisplay, image->getCameraMatrix(), posesForImage);

}

void PoseViewer::connectModelManagerSlots() {
    connect(modelManager, SIGNAL(poseAdded(QString)),
               this, SLOT(onPoseAdded(QString)));
    connect(modelManager, SIGNAL(poseDeleted(QString)),
               this, SLOT(onPoseDeleted(QString)));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(posesChanged()), this, SLOT(onPosesChanged()));
}

void PoseViewer::reset() {
    qDebug() << "Resetting pose viewer.";
    poseViewer3DWidget->reset();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
    currentlyDisplayedImage.reset();
}

void PoseViewer::reloadPoses() {
    if (!currentlyDisplayedImage.isNull()) {
        Image image = *(currentlyDisplayedImage.data());
        setImage(&image);
    } else {
        reset();
    }
}

void PoseViewer::visualizeLastClickedPosition(int posePointIndex) {
    Q_ASSERT(posePointIndex >= 0);
    /*
    ui->openGLWidget->addClick(lastClickedPosition,
              DisplayHelper::colorForPosePointIndex(posePointIndex));
              */
}

void PoseViewer::onPoseCreationAborted() {
    removePositionVisualizations();
}

void PoseViewer::removePositionVisualizations() {
    //ui->openGLWidget->removeClicks();
}

void PoseViewer::onPosePointStarted(QPoint /* point2D */,
                                                         int currentNumberOfPoints,
                                                         int /* minimumNumberOfPoints */) {
    // We can use the number of points as index directly, because the number of points only increases
    // after the user successfully clicked a 2D location and the corresponding 3D point
    visualizeLastClickedPosition(currentNumberOfPoints);
}

void PoseViewer::onPoseUpdated(Pose *pose){
    //ui->openGLWidget->updatePose(*pose);
}

void PoseViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;

    if (showingNormalImage)
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteImagePath(),
                                             currentlyDisplayedImage->getCameraMatrix());
    else
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteSegmentationImagePath(),
                                             currentlyDisplayedImage->getCameraMatrix());

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void PoseViewer::resetPositionOfGraphicsView() {
    poseViewer3DWidget->setGeometry(0, 0, poseViewer3DWidget->width(), poseViewer3DWidget->height());
}

void PoseViewer::onImageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->getImagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    lastClickedPosition = point;
    Q_EMIT imageClicked(currentlyDisplayedImage.data(), point);
}

void PoseViewer::onPoseDeleted(const QString &id) {
    poseViewer3DWidget->removePose(id);
}

void PoseViewer::onPoseAdded(const QString &id) {
    QSharedPointer<Pose> pose = modelManager->getPoseById(id);
    if (!pose.isNull())
        poseViewer3DWidget->addPose(*pose.data());
    poseViewer3DWidget->removeClicks();
}

void PoseViewer::onPosesChanged() {
    reloadPoses();
}

void PoseViewer::onImagesChanged() {
    reset();
}

void PoseViewer::onObjectModelsChanged() {
    reset();
}
