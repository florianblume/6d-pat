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
#include <Qt3DExtras/QTorusMesh>
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

    setupRenderingPipeline();
}

CorrespondenceViewer::~CorrespondenceViewer()
{
    deleteSceneObjects();
    delete ui;
}

void CorrespondenceViewer::deleteSceneObjects() {
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
        sceneObjectsEntity = Q_NULLPTR;
    }
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

void CorrespondenceViewer::setupSceneRoot() {
    // This recursively deletes all objects that are children of the entity
    if (sceneObjectsEntity) {
        delete sceneObjectsEntity;
    }
    sceneObjectsEntity = new Qt3DCore::QEntity(sceneRoot);
}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
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

void CorrespondenceViewer::showImage(const QString &imagePath) {
    setupRenderingPipeline();
    reset();
    // Set root entity here as parent so that image is a child of it
    qDebug() << "Displaying image (" + imagePath + ").";

    QList<ObjectImageCorrespondence> correspondencesForImage =
            modelManager->getCorrespondencesForImage(modelManager->getImages().at(currentlyDisplayedImageIndex));

    int i = 0;
    for (ObjectImageCorrespondence &correspondence : correspondencesForImage) {
        addObjectModelRenderable(correspondence, i);
        i++;
    }

    // This is just to retrieve the size of the set image
    QImage image(imagePath);
    ui->labelGraphics->setFixedSize(image.size());
    // Relation between camera matrix and field of view implies the following computation
    camera->setFieldOfView(2.f * std::atan(image.height() / (2.f * currentlyDisplayedImage->getFocalLengthX())) * (180.0f / M_PI));
    // Not necessary to set size first but can't hurt
    offscreenEngine->setSize(QSize(image.width(), image.height()));
    renderCaptureReply = offscreenEngine->getRenderCapture()->requestCapture();
    connect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
}

void CorrespondenceViewer::setImage(int index) {
    const QList<Image> &images = modelManager->getImages();
    Q_ASSERT(index < images.size());
    Q_ASSERT(index >= 0);
    currentlyDisplayedImageIndex = index;
    currentlyDisplayedImage.reset(new Image(images.at(index)));
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
        updateDisplayedImage();
        disconnect(renderCaptureReply, SIGNAL(completed()), this, SLOT(imageCaptured()));
        delete renderCaptureReply;
        imageReady = false;
    }
}

QImage CorrespondenceViewer::createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage) {
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(currentlyDisplayedImage->getFocalPointX() - (overlayImage.width() / 2.f) + 21.f,
                      currentlyDisplayedImage->getFocalPointY() - (overlayImage.height() / 2.f),
                      overlayImage);

    painter.end();

    return imageWithOverlay;
}

void CorrespondenceViewer::updateDisplayedImage() {
    QString baseImage = showingNormalImage ?
                                  currentlyDisplayedImage->getAbsoluteImagePath() :
                                  currentlyDisplayedImage->getAbsoluteSegmentationImagePath();
    QImage image =  createImageWithOverlay(QImage(baseImage), renderedImage);
    ui->labelGraphics->setPixmap(QPixmap::fromImage(image));
}

void CorrespondenceViewer::updateCorrespondence(ObjectImageCorrespondence correspondence) {
    reload();
}

void CorrespondenceViewer::removeCorrespondence(ObjectImageCorrespondence correspondence) {
    // This is easier than removing the correspondence and updating all the indeces that
    // correspond to the models
    reload();
}

void CorrespondenceViewer::reset() {
    qDebug() << "Resetting correspondence viewer.";
    deleteSceneObjects();
    setupSceneRoot();
}

void CorrespondenceViewer::reload() {
    setImage(currentlyDisplayedImageIndex);
}

void CorrespondenceViewer::switchImage() {
    const QList<Image> &images = modelManager->getImages();
    Q_ASSERT(currentlyDisplayedImageIndex < images.size());
    Q_ASSERT(currentlyDisplayedImageIndex >= 0);
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;
    //updateDisplayedImage();
    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::imageClicked(QPoint point) {
    emit imageClicked(currentlyDisplayedImage.get(), point);
}
