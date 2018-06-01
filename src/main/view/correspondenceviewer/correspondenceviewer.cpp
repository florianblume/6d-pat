#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
#include "view/misc/displayhelper.h"
#include "view/rendering/objectmodelrenderable.hpp"

#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <QOffscreenSurface>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>

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

    setupRenderingPipeline();

    if (modelManager) {
        connectModelManagerSlots();
    }
}

CorrespondenceViewer::~CorrespondenceViewer() {
    deleteSceneObjects();
    delete offscreenEngine;
    delete ui;
}

void CorrespondenceViewer::setupRenderingPipeline() {
    qDebug() << "Setting up rendering pipeline.";

    sceneRoot = new Qt3DCore::QEntity();

    sceneObjectsEntity = new Qt3DCore::QEntity(sceneRoot);

    // Setup camera
    camera = new Qt3DRender::QCamera(sceneRoot);
    // Initial projection matrix, the matrix will be updated as soon as an image is set
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 2000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 0.f));
    camera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    // Set view center z coordinate to 1.f to make the camera look along the z axis
    camera->setViewCenter(QVector3D(0.f, 0.f, 1.f));

    objectsLayer = new Qt3DRender::QLayer(sceneRoot);

    offscreenEngine = new OffscreenEngine(camera, QSize(500, 500), QPointF(0, 0));
    offscreenEngine->setSceneRoot(sceneRoot);
    offscreenEngine->addLayerToObjectsLayerFilter(objectsLayer);
}

void CorrespondenceViewer::deleteSceneObjects() {
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
        sceneObjectsEntity = Q_NULLPTR;
        objectModelRenderables.clear();
    }
}

void CorrespondenceViewer::setupSceneRoot() {
    // This recursively deletes all objects that are children of the entity
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
        objectModelRenderables.clear();
    }
    sceneObjectsEntity = new Qt3DCore::QEntity(sceneRoot);

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

    ui->buttonResetPosition->setEnabled(true);

    qDebug() << "Setting image (" + currentlyDisplayedImage->getImagePath() + ") to display.";

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
    showImage(showingNormalImage ?  currentlyDisplayedImage->getAbsoluteImagePath() :
                                    currentlyDisplayedImage->getAbsoluteSegmentationImagePath());
}

static QVector3D rotatePoint(const QVector3D point, const QVector3D &rotationVector) {
    QMatrix4x4 m;
    m.rotate(rotationVector.x(), QVector3D(1.0, 0.0, 0.0));
    m.rotate(rotationVector.y(), QVector3D(0.0, 1.0, 0.0));
    m.rotate(rotationVector.z(), QVector3D(0.0, 0.0, 1.0));
    return m * point;
}

void CorrespondenceViewer::showImage(const QString &imagePath) {
    deleteSceneObjects();
    setupSceneRoot();

    // Set root entity here as parent so that image is a child of it
    qDebug() << "Displaying image (" + imagePath + ").";

    // Do not use the pointer here as getCorrespondencesForImage requires a reference not a pointer
    QList<ObjectImageCorrespondence> correspondencesForImage =
            modelManager->getCorrespondencesForImage(*currentlyDisplayedImage.get());

    int i = 0;
    for (ObjectImageCorrespondence &correspondence : correspondencesForImage) {
        addObjectModelRenderable(correspondence, i);
        i++;
    }

    // This is just to retrieve the size of the set image
    QImage image(imagePath);
    offscreenEngine->setBackgroundImagePath(imagePath);
    ui->labelGraphics->setFixedSize(image.size());

    // TODO: incorporate camera rotation and position
    //camera->setPosition(currentlyDisplayedImage->getCameraPosition());
    //QVector3D rotatedViewCenter = rotatePoint(camera->viewCenter(), currentlyDisplayedImage->getCameraRotation());
    //camera->setViewCenter(rotatedViewCenter + camera->position());
    //QVector3D rotatedUpVector = rotatePoint(camera->upVector(), currentlyDisplayedImage->getCameraRotation());
    //camera->setUpVector(rotatedUpVector + camera->upVector());

    // Relation between camera matrix and field of view implies the following computation
    // 180.f / M_PI is conversion from radians to degrees
    camera->setFieldOfView(2.f * std::atan(image.height() /
                                           (2.f * currentlyDisplayedImage->getFocalLengthX())) * (180.0f / M_PI));
    // Not necessary to set size first but can't hurt
    offscreenEngine->setSize(QSize(image.width(), image.height()));
    float objectsXOffset = ((image.width() / 2.f) - (float) currentlyDisplayedImage->getFocalPointX())
                                                                / (float) image.width();
    float objectsYOffset = ((image.height() / 2.f) - (float) currentlyDisplayedImage->getFocalPointY())
                                                                / (float) image.height();
    offscreenEngine->setObjectsOffset(QPointF(objectsXOffset, objectsYOffset));
    renderAgain += 1;
    Qt3DRender::QRenderCaptureReply *renderCaptureReply = offscreenEngine->getRenderCapture()->requestCapture();
    renderReplies.append(renderCaptureReply);
    connect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
}

void CorrespondenceViewer::addObjectModelRenderable(const ObjectImageCorrespondence &correspondence,
                                                    int objectModelIndex) {
    const ObjectModel* objectModel = correspondence.getObjectModel();
    // We do not need to take care of deleting the renderables, the destructor of this class
    // or the start of this function will do this
    ObjectModelRenderable *newRenderable =
            new ObjectModelRenderable(sceneObjectsEntity,
                                      objectModel->getAbsolutePath(), "");

    newRenderable->getTransform()->setTranslation(QVector3D(correspondence.getPosition().x(),
                                                       correspondence.getPosition().y(),
                                                       correspondence.getPosition().z()));
    newRenderable->getTransform()->setRotationX(correspondence.getRotation().x());
    newRenderable->getTransform()->setRotationY(correspondence.getRotation().y());
    newRenderable->getTransform()->setRotationZ(correspondence.getRotation().z());
    newRenderable->addComponent(objectsLayer);
    Qt3DExtras::QPhongMaterial *phongAlphaMaterial = new Qt3DExtras::QPhongMaterial(newRenderable);
    phongAlphaMaterial->setAmbient(QColor(100, 100, 100, objectsOpacity));
    newRenderable->addComponent(phongAlphaMaterial);
    objectModelRenderables.insert(correspondence.getID(), newRenderable);

    qDebug() << "Adding object model (" + objectModel->getPath() + ") to display.";
}

void CorrespondenceViewer::imageCaptured() {
    Qt3DRender::QRenderCaptureReply *renderCaptureReply = renderReplies.at(0);
    renderedImage = renderCaptureReply->image().mirrored(true, true);
    renderedImageDefault = renderedImage;
    disconnect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
    renderReplies.pop_front();
    delete renderCaptureReply;
    ui->labelGraphics->setPixmap(QPixmap::fromImage(renderedImage));
    if (renderAgain > 0) {
        renderAgain--;
        Qt3DRender::QRenderCaptureReply *renderCaptureReply = offscreenEngine->getRenderCapture()->requestCapture();
        renderReplies.append(renderCaptureReply);
        connect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
    }
}

void CorrespondenceViewer::connectModelManagerSlots() {
    connect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(refresh()));
    connect(modelManager, SIGNAL(correspondenceAdded(QString)), this, SLOT(refresh()));
    connect(modelManager, SIGNAL(correspondenceDeleted(QString)), this, SLOT(onCorrespondenceRemoved(QString)));
    connect(modelManager, SIGNAL(correspondenceUpdated(QString)), this, SLOT(onCorrespondenceUpdated(QString)));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
}

void CorrespondenceViewer::reset() {
    qDebug() << "Resetting correspondence viewer.";
    deleteSceneObjects();
    setupSceneRoot();
    currentlyDisplayedImage.release();
    ui->labelGraphics->setPixmap(QPixmap(0, 0));
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
}

void CorrespondenceViewer::refresh() {
    if (currentlyDisplayedImage.get())
        setImage(currentlyDisplayedImage.get());
}

void CorrespondenceViewer::visualizeLastClickedPosition(int correspondencePointIndex) {
    Q_ASSERT(correspondencePointIndex >= 0);
    QPainter painter(&renderedImage);
    QPen paintpen(DisplayHelper::colorForCorrespondencePointIndex(correspondencePointIndex));
    paintpen.setWidth(3);
    painter.setPen(paintpen);
    painter.drawPoint(lastClickedPosition);
    ui->labelGraphics->setPixmap(QPixmap::fromImage(renderedImage));
}

void CorrespondenceViewer::onCorrespondenceCreationAborted() {
    removePositionVisualizations();
}

void CorrespondenceViewer::removePositionVisualizations() {
    renderedImage = renderedImageDefault;
    ui->labelGraphics->setPixmap(QPixmap::fromImage(renderedImageDefault));
}

void CorrespondenceViewer::onCorrespondencePointStarted(QPoint point2D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    // We can use the number of points as index directly, because the number of points only increases
    // after the user successfully clicked a 2D location and the corresponding 3D point
    visualizeLastClickedPosition(currentNumberOfPoints);
}

void CorrespondenceViewer::onOpacityForObjectModelsChanged(int opacity) {
    objectsOpacity = opacity / 100.f;
    refresh();
}

void CorrespondenceViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;

    refresh();

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::resetPositionOfImage() {
    ui->labelGraphics->setGeometry(0, 0,
                                   ui->labelGraphics->geometry().width(),
                                   ui->labelGraphics->geometry().height());
}

void CorrespondenceViewer::imageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->getImagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    lastClickedPosition = point;
    emit imageClicked(currentlyDisplayedImage.get(), point);
}

void CorrespondenceViewer::onCorrespondenceUpdated(const QString &id) {
    if (objectModelRenderables.contains(id)) {
        ObjectImageCorrespondence correspondence = modelManager->getCorrespondenceById(id);
        ObjectModelRenderable *renderable = objectModelRenderables.value(id);
        renderable->getTransform()->setTranslation(QVector3D(correspondence.getPosition().x(),
                                                           correspondence.getPosition().y(),
                                                           correspondence.getPosition().z()));
        renderable->getTransform()->setRotationX(correspondence.getRotation().x());
        renderable->getTransform()->setRotationY(correspondence.getRotation().y());
        renderable->getTransform()->setRotationZ(correspondence.getRotation().z());
        refresh();
    } else {
        throw "A correspondence has been edited, that is currently not part of the viewer.";
    }
}

void CorrespondenceViewer::onCorrespondenceRemoved(const QString &id) {
    if (objectModelRenderables.contains(id)) {
        ObjectModelRenderable *renderable = objectModelRenderables.value(id);
        renderable->setParent(new Qt3DCore::QEntity());
        objectModelRenderables.remove(id);
        delete renderable;
        refresh();
    } else {
        throw "A correspondence has been removed, that is currently not part of the viewer.";
    }
}
