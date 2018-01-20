#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
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

    ui->buttonAccept->setFont(awesome->font(18));
    ui->buttonAccept->setIcon(awesome->icon(fa::check));
    ui->buttonAccept->setToolTip("Click to accept the correspondences.\n"
                                 "They will be taken into account when updating the neural network.");
    ui->buttonAccept->setEnabled(false);

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
        connect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(update()));
        connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
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
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.01f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 0.f));
    camera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    // Set view center z coordinate to 1.f to make the camera look along the z axis
    camera->setViewCenter(QVector3D(0.f, 0.f, 1.f));

    lightEntity = new Qt3DCore::QEntity(sceneRoot);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(camera->position());
    lightEntity->addComponent(lightTransform);

    offscreenEngine = new OffscreenEngine(camera, QSize(500, 500));
    offscreenEngine->setSceneRoot(sceneRoot);
}

void CorrespondenceViewer::deleteSceneObjects() {
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
        sceneObjectsEntity = Q_NULLPTR;
    }
}

void CorrespondenceViewer::setupSceneRoot() {
    // This recursively deletes all objects that are children of the entity
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
    }
    sceneObjectsEntity = new Qt3DCore::QEntity(sceneRoot);

}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    if (this->modelManager) {
        disconnect(this->modelManager, SIGNAL(correspondencesChanged()),
                   this, SLOT(update()));
        disconnect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        disconnect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connect(modelManager, SIGNAL(correspondencesChanged()), this, SLOT(update()));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
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
    ui->labelGraphics->setFixedSize(image.size());
    // Relation between camera matrix and field of view implies the following computation
    // 180.f / M_PI is conversion from radians to degrees
    camera->setFieldOfView(2.f * std::atan(image.height() /
                                           (2.f * currentlyDisplayedImage->getFocalLengthX())) * (180.0f / M_PI));
    // Not necessary to set size first but can't hurt
    offscreenEngine->setSize(QSize(image.width(), image.height()));
    renderCaptureReply = offscreenEngine->getRenderCapture()->requestCapture();
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

    qDebug() << "Adding object model (" + objectModel->getPath() + ") to display.";
}

void CorrespondenceViewer::imageCaptured() {
    // The first image is somehow always empty, so request another capture and save that

    if (!imageReady) {
        disconnect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
        delete renderCaptureReply;
        renderCaptureReply = offscreenEngine->getRenderCapture()->requestCapture();
        connect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
        imageReady = true;
    } else {
        renderedImage = renderCaptureReply->image().mirrored(true, true);
        // Sanity check here, because rendering is done asynchronously
        // There is a case for example, when the user switches the images path, that the image is
        // rendered. Inbetween, reset() is called from the onPreferencesChanged() method in the
        // main view. Thus the index has been reset and we should not display the rendered image.
        if (currentlyDisplayedImage.get() != Q_NULLPTR)
            updateDisplayedImage();
        disconnect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
        delete renderCaptureReply;
        imageReady = false;
    }
}

void CorrespondenceViewer::updateDisplayedImage() {
    QString baseImage = showingNormalImage ?
                                  currentlyDisplayedImage->getAbsoluteImagePath() :
                                  currentlyDisplayedImage->getAbsoluteSegmentationImagePath();
    composedImage =  createImageWithOverlay(QImage(baseImage), renderedImage);
    ui->labelGraphics->setPixmap(QPixmap::fromImage(composedImage));
}

QImage CorrespondenceViewer::createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage) {
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Account for the offset of the focal point
    painter.drawImage(currentlyDisplayedImage->getFocalPointX() - (overlayImage.width() / 2.f) + 21.f,
                      currentlyDisplayedImage->getFocalPointY() - (overlayImage.height() / 2.f),
                      overlayImage);

    painter.end();

    return imageWithOverlay;
}

void CorrespondenceViewer::reset() {
    qDebug() << "Resetting correspondence viewer.";
    deleteSceneObjects();
    setupSceneRoot();
    currentlyDisplayedImage.release();
    ui->labelGraphics->setPixmap(QPixmap(0, 0));
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonAccept->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
}

void CorrespondenceViewer::update() {
    if (currentlyDisplayedImage.get())
        setImage(currentlyDisplayedImage.get());
}

void CorrespondenceViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;

    update();

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
    emit imageClicked(currentlyDisplayedImage.get(), point);
}
