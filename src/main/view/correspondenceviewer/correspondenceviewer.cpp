#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
#include "view/misc/displayhelper.h"

#include "math.h"

CorrespondenceViewer::CorrespondenceViewer(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceViewer),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager)
{
    ui->setupUi(this);

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

    connect(ui->openGLWidget, SIGNAL(positionClicked(QPoint)), this, SLOT(onImageClicked(QPoint)));
}

CorrespondenceViewer::~CorrespondenceViewer() {
    delete ui;
}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    if (this->modelManager) {
        disconnect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(onCorrespondencesChanged()));
        disconnect(modelManager, SIGNAL(correspondenceAdded(QString)),
                   this, SLOT(onCorrespondenceAdded(QString)));
        disconnect(modelManager, SIGNAL(correspondenceDeleted(QString)),
                   this, SLOT(onCorrespondenceDeleted(QString)));
        disconnect(this->modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        disconnect(this->modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connectModelManagerSlots();
}

Image *CorrespondenceViewer::getCurrentlyViewedImage() {
    return currentlyDisplayedImage.data();
}

void CorrespondenceViewer::setImage(Image *image) {
    currentlyDisplayedImage.reset(new Image(*image));

    ui->openGLWidget->removeCorrespondences();
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
    QList<Correspondence> correspondencesForImage = modelManager->getCorrespondencesForImage(*image);
    ui->openGLWidget->setBackgroundImageAndCorrespondences(toDisplay,
                                                           image->getCameraMatrix(),
                                                           correspondencesForImage);

}

void CorrespondenceViewer::connectModelManagerSlots() {
    connect(modelManager, SIGNAL(correspondenceAdded(QString)),
               this, SLOT(onCorrespondenceAdded(QString)));
    connect(modelManager, SIGNAL(correspondenceDeleted(QString)),
               this, SLOT(onCorrespondenceDeleted(QString)));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(onCorrespondencesChanged()));
}

void CorrespondenceViewer::updateOpacity(){
    ui->openGLWidget->setObjectsOpacity(objectsOpacity);
}

void CorrespondenceViewer::reset() {
    qDebug() << "Resetting correspondence viewer.";
    ui->openGLWidget->reset();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
    currentlyDisplayedImage.reset();
}

void CorrespondenceViewer::reloadCorrespondences() {
    if (!currentlyDisplayedImage.isNull()) {
        Image image = *(currentlyDisplayedImage.data());
        setImage(&image);
    } else {
        reset();
    }
}

void CorrespondenceViewer::visualizeLastClickedPosition(int correspondencePointIndex) {
    Q_ASSERT(correspondencePointIndex >= 0);
    ui->openGLWidget->addClick(lastClickedPosition,
              DisplayHelper::colorForCorrespondencePointIndex(correspondencePointIndex));
}

void CorrespondenceViewer::onCorrespondenceCreationAborted() {
    removePositionVisualizations();
}

void CorrespondenceViewer::removePositionVisualizations() {
    ui->openGLWidget->removeClicks();
}

void CorrespondenceViewer::onCorrespondencePointStarted(QPoint /* point2D */,
                                                         int currentNumberOfPoints,
                                                         int /* minimumNumberOfPoints */) {
    // We can use the number of points as index directly, because the number of points only increases
    // after the user successfully clicked a 2D location and the corresponding 3D point
    visualizeLastClickedPosition(currentNumberOfPoints);
}

void CorrespondenceViewer::onCorrespondenceUpdated(Correspondence *correspondence){
    ui->openGLWidget->updateCorrespondence(*correspondence);
}

void CorrespondenceViewer::onOpacityChangeStarted(int opacity) {
    objectsOpacity = opacity / 100.f;
    if (!opacityTimer) {
        opacityTimer = new QTimer();
        connect(opacityTimer, SIGNAL(timeout()), this, SLOT(updateOpacity()));
        // Update opacity only every 30 ms
        opacityTimer->start(30);
    }
}

void CorrespondenceViewer::onOpacityChangeEnded() {
    if (opacityTimer) {
        // Sometimes the timer is delete already...
        opacityTimer->stop();
        disconnect(opacityTimer, SIGNAL(timeout()), this, SLOT(updateOpacity()));
        delete opacityTimer;
        opacityTimer = 0;
    }
}

void CorrespondenceViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;
    if (showingNormalImage)
        ui->openGLWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteImagePath(),
                                             currentlyDisplayedImage->getCameraMatrix());
    else
        ui->openGLWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteSegmentationImagePath(),
                                             currentlyDisplayedImage->getCameraMatrix());

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::resetPositionOfGraphicsView() {
    QRect geo = ui->openGLWidget->geometry();
    ui->openGLWidget->setGeometry(0, 0, geo.width(), geo.height());
}

void CorrespondenceViewer::onImageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->getImagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    lastClickedPosition = point;
    Q_EMIT imageClicked(currentlyDisplayedImage.data(), point);
}

void CorrespondenceViewer::onCorrespondenceDeleted(const QString &id) {
    ui->openGLWidget->removeCorrespondence(id);
}

void CorrespondenceViewer::onCorrespondenceAdded(const QString &id) {
    QSharedPointer<Correspondence> correspondence = modelManager->getCorrespondenceById(id);
    if (!correspondence.isNull())
        ui->openGLWidget->addCorrespondence(*correspondence.data());
    ui->openGLWidget->removeClicks();
}

void CorrespondenceViewer::onCorrespondencesChanged() {
    reloadCorrespondences();
}

void CorrespondenceViewer::onImagesChanged() {
    reset();
}

void CorrespondenceViewer::onObjectModelsChanged() {
    reset();
}
