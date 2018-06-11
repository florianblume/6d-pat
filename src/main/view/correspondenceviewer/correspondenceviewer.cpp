#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
#include "view/misc/displayhelper.h"
#include "view/rendering/opengl/objectmodelrenderable.hpp"

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
}

CorrespondenceViewer::~CorrespondenceViewer() {
    delete ui;
}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    if (this->modelManager) {
        disconnect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(refresh()));
        disconnect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT());
        disconnect(modelManager, SIGNAL(correspondenceDeleted(QString)), this, SLOT(onCorrespondenceRemoved(QString)));
        disconnect(modelManager, SIGNAL(correspondenceUpdated(QString)), this, SLOT(onCorrespondenceUpdated(QString)));
        disconnect(this->modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        disconnect(this->modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connectModelManagerSlots();
}

void CorrespondenceViewer::setImage(Image *image) {

    currentlyDisplayedImage.reset(new Image(*image));

    ui->openGLWidget->removeObjectModels();
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
    ui->openGLWidget->setBackgroundImage(toDisplay, image->getCameraMatrix());
    QList<Correspondence> correspondencesForImage = modelManager->getCorrespondencesForImage(*image);
    for (Correspondence &correspondence : correspondencesForImage) {
        ui->openGLWidget->addCorrespondence(correspondence);
    }
}

void CorrespondenceViewer::connectModelManagerSlots() {
    // We get notified directly from the Editor so need to listen to the manager here
    //connect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(refresh()));
    //connect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT(refresh()));
    //connect(modelManager, SIGNAL(correspondenceDeleted(QString)), this, SLOT(onCorrespondenceRemoved(QString)));
    //connect(modelManager, SIGNAL(correspondenceUpdated(QString)), this, SLOT(onCorrespondenceUpdated(QString)));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
}

void CorrespondenceViewer::reset() {
    qDebug() << "Resetting correspondence viewer.";
    ui->openGLWidget->removeObjectModels();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
}

void CorrespondenceViewer::visualizeLastClickedPosition(int correspondencePointIndex) {
    Q_ASSERT(correspondencePointIndex >= 0);
    // TODO: show clicks on open gl widget
}

void CorrespondenceViewer::onCorrespondenceCreationAborted() {
    removePositionVisualizations();
}

void CorrespondenceViewer::removePositionVisualizations() {
    // TOOD: remove clicks on open gl widget
}

void CorrespondenceViewer::onCorrespondencePointStarted(QPoint point2D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    // We can use the number of points as index directly, because the number of points only increases
    // after the user successfully clicked a 2D location and the corresponding 3D point
    visualizeLastClickedPosition(currentNumberOfPoints);
}

void CorrespondenceViewer::onCorrespondenceUpdate(Correspondence *correspondence){
    // TODO: update correspondence
}

void CorrespondenceViewer::onOpacityForObjectModelsChanged(int opacity) {
    objectsOpacity = opacity / 100.f;
    ui->openGLWidget->setOpacity(objectsOpacity);
}

void CorrespondenceViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;
    // TODO: update open gl widget

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::resetPositionOfGraphicsView() {
    //QRect geometry = ui->gr;
}

void CorrespondenceViewer::imageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->getImagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    lastClickedPosition = point;
    emit imageClicked(currentlyDisplayedImage.get(), point);
}

void CorrespondenceViewer::onCorrespondenceRemoved(const QString &id) {

}
