#include "correspondenceviewer.hpp"
#include "ui_correspondenceviewer.h"
#include "view/rendering/imagerenderable.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QFirstPersonCameraController>

CorrespondenceViewer::CorrespondenceViewer(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceViewer),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager)
{
    ui->setupUi(this);

    //! Connect the mapper that is going to map the signals of the object models pickers
    //! to the index function to be able to determine which picker was clicked
    connect(objectModelPickerSignalMapper.data(), SIGNAL(mapped(int)),
            this, SLOT(objectModelObjectPickerPressed(int)));

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

    setupGraphicsWindow();
}

CorrespondenceViewer::~CorrespondenceViewer()
{
    graphicsWindow->destroy();
    deleteSceneObjects();
    delete graphicsWindow;
    delete ui;
}

void CorrespondenceViewer::deleteSceneObjects() {
    // It's a bit confusing of what the 3D window takes ownership and of what not...
    // This is why we use the QPointer class to be able to surely tell when a pointer
    // is null and thus we should not try to delet it
    if (imageRenderable)
        delete imageRenderable;
    if (imageObjectPicker)
        delete imageObjectPicker;
    for (ObjectModelRenderable* renderable : objectModelRenderables) {
        if (renderable)
            delete renderable;
    }
    for (Qt3DRender::QObjectPicker *picker : objectModelsPickers) {
        if (picker)
            delete picker;
    }
}

void CorrespondenceViewer::setupGraphicsWindow() {
    qDebug() << "Setting up graphics window.";
    // Create the container for our 3D window that is going to display the image and objects
    graphicsWindow = new Qt3DExtras::Qt3DWindow;
    QWidget* containerWidget = QWidget::createWindowContainer(graphicsWindow);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->frameGraphics->layout()->addWidget(containerWidget);

    // Setup camera
    Qt3DRender::QCamera *camera = graphicsWindow->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 1.5f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Setup camera control -> Remove later!
    Qt3DExtras::QFirstPersonCameraController *firstPerson = new Qt3DExtras::QFirstPersonCameraController(graphicsWindow->activeFrameGraph());
    firstPerson->setCamera(camera);

    // Setup root node
    rootEntity = new Qt3DCore::QEntity();
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    frameGraph = new Qt3DRender::QRenderSettings();
    frameGraph->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    rootEntity->addComponent(frameGraph);
    depthTest = new Qt3DRender::QDepthTest(frameGraph);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
    frameGraph->setActiveFrameGraph(graphicsWindow->activeFrameGraph());
    graphicsWindow->setRootEntity(rootEntity);
}

void CorrespondenceViewer::setupSceneRoot() {
    delete sceneEntity;
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
}

void CorrespondenceViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
}

void CorrespondenceViewer::showImage(const QString &imagePath) {
    // Set root entity here as parent so that image is a child of it
    qDebug() << "Displaying image (" + imagePath + ").";
    deleteSceneObjects();
    setupSceneRoot();

    // Clear the correspondences
    correspondences.clear();

    // Create the image plane that displays the image
    imageRenderable = new ImageRenderable(sceneEntity, imagePath);
    imageObjectPicker = new Qt3DRender::QObjectPicker();
    imageRenderable->addComponent(imageObjectPicker);

    // We want to catch picking events to pass on to the main window to be able to detect
    // when the user wants to create a new correspondence
    connect(imageObjectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)),
            this, SLOT(imageObjectPickerPressed(Qt3DRender::QPickEvent*)));
    QList<ObjectImageCorrespondence*> correspondencesForImage;
    modelManager->getCorrespondencesForImage(*modelManager->getImage(currentlyDisplayedImage),
                                             correspondencesForImage);
    int i = 0;
    for (ObjectImageCorrespondence* correspondence : correspondencesForImage) {
        addObjectModelRenderable(correspondence, i);
        i++;
    }
}

void CorrespondenceViewer::addObjectModelRenderable(ObjectImageCorrespondence *correspondence,
                                                    int objectModelIndex) {
    const ObjectModel* objectModel = correspondence->getObjectModel();
    // We do not need to take care of deleting the renderables, the destructor of this class
    // or the start of this function will do this
    ObjectModelRenderable *newRenderable =
            new ObjectModelRenderable(sceneEntity,
                                      objectModel->getAbsolutePath(), "");

    qDebug() << "Adding object model (" + objectModel->getPath() + ") to display.";

    objectModelRenderables.push_back(newRenderable);
    // We store the correspondences to be able to retrieve them when the object belonging to
    // the correspondence is clicked
    //correspondences.push_back(correspondence);

    // We do this so that wenn someone calls the update correspondence or remove correspondence
    // function we are able to retrieve the 3D model and adjust it
    objectModelToRenderablePointerMap.insert(objectModel, newRenderable);
    Qt3DRender::QObjectPicker *picker = new Qt3DRender::QObjectPicker(newRenderable);
    newRenderable->addComponent(picker);
    objectModelsPickers.push_back(picker);

    // Map the picking event to the respective index
    connect(picker, SIGNAL(pressed(Qt3DRender::QPickEvent*)),
            objectModelPickerSignalMapper.data(), SLOT(map()));
    objectModelPickerSignalMapper->setMapping(picker, objectModelIndex);
}

void CorrespondenceViewer::setImage(int index) {
    Q_ASSERT(index < modelManager->getImagesSize());
    Q_ASSERT(index >= 0);
    currentlyDisplayedImage = index;
    const Image* image = modelManager->getImage(index);
    qDebug() << "Setting image (" + image->getImagePath() + ") to display.";

    // Enable/disable functionality to show only segmentation image instead of normal image
    if (image->getSegmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        // If we don't find a segmentation image, set that we will now display the normal image
        // because the formerly set image could have had a segmentation image and set this value
        // to false
        showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    showImage(showingNormalImage ?  image->getAbsoluteImagePath() : image->getAbsoluteSegmentationImagePath());
}

void CorrespondenceViewer::updateCorrespondence(ObjectImageCorrespondence* correspondence) {
    reload();
}

void CorrespondenceViewer::removeCorrespondence(ObjectImageCorrespondence* correspondence) {
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
    setImage(currentlyDisplayedImage);
}

void CorrespondenceViewer::switchImage() {
    Q_ASSERT(currentlyDisplayedImage < modelManager->getImagesSize());
    Q_ASSERT(currentlyDisplayedImage >= 0);
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    const Image* image = modelManager->getImage(currentlyDisplayedImage);
    showImage(showingNormalImage ? image->getAbsoluteSegmentationImagePath() : image->getAbsoluteImagePath());
    showingNormalImage = !showingNormalImage;
    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void CorrespondenceViewer::imageObjectPickerPressed(Qt3DRender::QPickEvent *pick) {
    if (pick->button() != Qt::LeftButton)
        return;

    QVector3D intersection = pick->worldIntersection();
    const Image* image = modelManager->getImage(currentlyDisplayedImage);
    QPointF point = QPointF(intersection.x(), intersection.y());
    qDebug() << "Image (" + image->getImagePath() + ") clicked at position: ("
                + QString::number(point.x()) + ", "
                + QString::number(point.y()) + ")";
    emit imageClicked(image, point);
}

void CorrespondenceViewer::objectModelObjectPickerPressed(int index) {
    //ObjectImageCorrespondence *correspondence = correspondences.at(index);
    //qDebug() << "Object model (" + correspondence->getObjectModel()->getPath() + ") clicked.";
    //emit correspondenceClicked(correspondence);
}
