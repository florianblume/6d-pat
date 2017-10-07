#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "view/rendering/imagerenderable.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QCamera>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager),
    graphicsWindow(new Qt3DExtras::Qt3DWindow)
{
    ui->setupUi(this);

    //! Connect the mapper that is going to map the signals of the object models pickers
    //! to the index function to be able to determine which picker was clicked
    connect(objectModelPickerSignalMapper.data(), SIGNAL(mapped(int)),
            this, SLOT(objectModelObjectPickerPressed(int)));

    //! Create the container for our 3D window that is going to display the image and objects
    QWidget* containerWidget = QWidget::createWindowContainer(graphicsWindow);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->frameGraphics->layout()->addWidget(containerWidget);

    Qt3DRender::QCamera *camera = graphicsWindow->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 1.5f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    setupRootEntity();

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
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    graphicsWindow->destroy();
    deleteObjects();
    delete graphicsWindow;
    delete ui;
}

void CorrespondenceEditor::setupRootEntity() {
    rootEntity = new Qt3DCore::QEntity();
    depthTest = new Qt3DRender::QDepthTest(rootEntity);
    graphicsWindow->setRootEntity(rootEntity);
    //! Always pass depth test i.e. objects will be visible above other objects
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
}

void CorrespondenceEditor::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    this->modelManager = modelManager;
}

void CorrespondenceEditor::deleteObjects() {
    //! It's a bit confusing of what the 3D window takes ownership and of what not...
    //! This is why we use the QPointer class to be able to surely tell when a pointer
    //! is null and thus we should not try to delet it
    if (depthTest)
        delete depthTest;
    if (rootEntity)
        delete rootEntity;
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

void CorrespondenceEditor::showImage(const QString &imagePath) {
    //! Set root entity here as parent so that image is a child of it
    deleteObjects();
    setupRootEntity();
    imageRenderable = new ImageRenderable(rootEntity, imagePath);
    imageObjectPicker = new Qt3DRender::QObjectPicker(imageRenderable);
    imageRenderable->addComponent(imageObjectPicker);

    //! We want to catch picking events to pass on to the main window to be able to detect
    //! when the user wants to create a new correspondence
    connect(imageObjectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)),
            this, SLOT(imageObjectPickerPressed(Qt3DRender::QPickEvent*)));
    QList<ObjectImageCorrespondence*> correspondencesForImage;
    modelManager->getCorrespondencesForImage(*modelManager->getImage(currentlyDisplayedImage),
                                             correspondencesForImage);
    int i = 0;
    for (ObjectImageCorrespondence* correspondence : correspondencesForImage) {
        addObjectModelRenderable(&correspondence->getObjectModel(), i);
        i++;
    }
}

void CorrespondenceEditor::addObjectModelRenderable(const ObjectModel* objectModel,
                                                    int objectModelIndex) {
    //! We do not need to take care of deleting the renderables, the destructor of this class
    //! or the start of this function will do this
    ObjectModelRenderable *newRenderable =
            new ObjectModelRenderable(rootEntity, objectModel->getAbsolutePath(), "");
    objectModelRenderables.push_back(newRenderable);

    //! We do this so that wenn someone calls the update correspondence or remove correspondence
    //! function we are able to retrieve the 3D model and adjust it
    objectModelToRenderablePointerMap.insert(objectModel, newRenderable);
    Qt3DRender::QObjectPicker *picker = new Qt3DRender::QObjectPicker(newRenderable);
    objectModelsPickers.push_back(picker);

    //! Map the picking event to the respective index
    connect(picker, SIGNAL(pressed(Qt3DRender::QPickEvent*)),
            objectModelPickerSignalMapper.data(), SLOT(map()));
    objectModelPickerSignalMapper->setMapping(picker, objectModelIndex);
}

void CorrespondenceEditor::setImage(int index) {
    Q_ASSERT(index < modelManager->getImagesSize());
    Q_ASSERT(index >= 0);
    currentlyDisplayedImage = index;
    const Image* image = modelManager->getImage(index);

    //! Enable/disable functionality to show only segmentation image instead of normal image
    if (image->getSegmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        //! If we don't find a segmentation image, set that we will now display the normal image
        //! because the formerly set image could have had a segmentation image and set this value
        //! to false
        showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    showImage(showingNormalImage ?  image->getAbsoluteImagePath() : image->getAbsoluteSegmentationImagePath());
}

void CorrespondenceEditor::updateCorrespondence(ObjectImageCorrespondence* correspondence) {
    if (objectModelToRenderablePointerMap.contains(&correspondence->getObjectModel())) {
        //! Object model is already displayed, we only need to update it
        // TODO: Update the object model
    } else {
        //! Oops, we didn't find the related object model, thus reload the complete view
        //! This is easier than keeping track of the objects... Maybe we will implement
        //! the simple addition later...
        reload();
    }
}

void CorrespondenceEditor::removeCorrespondence(ObjectImageCorrespondence* correspondence) {
    //! This is easier than removing the correspondence and updating all the indeces that
    //! correspond to the models
    reload();
}

void CorrespondenceEditor::reset() {
    deleteObjects();
    setupRootEntity();
}

void CorrespondenceEditor::reload() {
    setImage(currentlyDisplayedImage);
}

void CorrespondenceEditor::switchImage() {
    Q_ASSERT(currentlyDisplayedImage < modelManager->getImagesSize());
    Q_ASSERT(currentlyDisplayedImage >= 0);
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    const Image* image = modelManager->getImage(currentlyDisplayedImage);
    showImage(showingNormalImage ? image->getAbsoluteSegmentationImagePath() : image->getAbsoluteImagePath());
    showingNormalImage = !showingNormalImage;
}

void CorrespondenceEditor::imageObjectPickerPressed(Qt3DRender::QPickEvent *pick) {
    QPointF position = pick->position();
    position.setX(position.x() + graphicsWindow->geometry().x());
    position.setY(position.y() + graphicsWindow->geometry().y());
    emit imageClicked(position);
}

void CorrespondenceEditor::objectModelObjectPickerPressed(int index) {
    const ObjectModel *model = modelManager->getObjectModel(index);
    emit objectModelClicked(model);
}
