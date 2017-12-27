#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
#include "view/rendering/imagerenderable.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <QOffscreenSurface>

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
    // It's a bit confusing of what the 3D window takes ownership and of what not...
    // This is why we use the QPointer class to be able to surely tell when a pointer
    // is null and thus we should not try to delet it
    if (sceneEntity) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
    }
}

void CorrespondenceViewer::setupRenderingPipeline() {
    qDebug() << "Setting up rendering pipeline.";

    // Setup root node
    sceneEntity = new Qt3DCore::QEntity();

    // Setup camera
    camera = new Qt3DRender::QCamera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 0.f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    offscreenEngine = new OffscreenEngine(camera, QSize(500, 500));
    offscreenEngine->setSceneRoot(sceneEntity);
}

void CorrespondenceViewer::setupSceneRoot() {
    delete sceneEntity;
    sceneEntity = new Qt3DCore::QEntity();
    offscreenEngine->setSceneRoot(sceneEntity);
}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
}

void CorrespondenceViewer::showImage(const QString &imagePath) {
    reset();
    // Set root entity here as parent so that image is a child of it
    qDebug() << "Displaying image (" + imagePath + ").";

    // Create the image plane that displays the image
    imageRenderable = new ImageRenderable(sceneEntity, imagePath);

    QList<ObjectImageCorrespondence> correspondencesForImage =
            modelManager->getCorrespondencesForImage(modelManager->getImages().at(currentlyDisplayedImageIndex));

    int i = 0;
    for (ObjectImageCorrespondence &correspondence : correspondencesForImage) {
        addObjectModelRenderable(correspondence, i);
        i++;
    }
    QImage image(imagePath);
    ui->labelGraphics->setFixedSize(image.size());
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
            new ObjectModelRenderable(sceneEntity,
                                      objectModel->getAbsolutePath(), "");

    qDebug() << "Adding object model (" + objectModel->getPath() + ") to display.";
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
    renderedImage = renderCaptureReply->image();
    updateDisplayedImage();
    delete renderCaptureReply;
}

QImage CorrespondenceViewer::createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage) {
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlayImage);

    painter.end();

    return imageWithOverlay;
}

void CorrespondenceViewer::updateDisplayedImage() {
    QString baseImage = showingNormalImage ?
                                  currentlyDisplayedImage->getAbsoluteImagePath() :
                                  currentlyDisplayedImage->getAbsoluteSegmentationImagePath();
    QImage image = createImageWithOverlay(QImage(baseImage), renderedImage);
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
    updateDisplayedImage();
    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::imageClicked(QPoint point) {
    emit imageClicked(currentlyDisplayedImage.get(), point);
}
