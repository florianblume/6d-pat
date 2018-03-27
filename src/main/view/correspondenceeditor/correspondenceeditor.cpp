#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "view/misc/displayhelper.h"
#include "view/rendering/objectmodelrenderable.hpp"

#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <QUrl>
#include <QThread>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent, ModelManager *modelManager) :
    QWidget(parent),
    modelManager(modelManager),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    setup3DView();
    if (modelManager)
        connect(modelManager, SIGNAL(correspondenceAdded()), this, SLOT(removePositionVisualizations()));
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    if (graphicsWindow) {
        graphicsWindow->destroy();
        delete graphicsWindow;
    }
    delete ui;
}

void CorrespondenceEditor::setModelManager(ModelManager *modelManager) {
    Q_ASSERT(modelManager);
    this->modelManager = modelManager;
}

void CorrespondenceEditor::setEnabledCorrespondenceEditorControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->sliderArticulation->setEnabled(enabled);
    // The next line is the difference to setEnabledAllControls
    ui->buttonRemove->setEnabled(enabled);
}

void CorrespondenceEditor::setEnabledAllControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->sliderArticulation->setEnabled(enabled);
    ui->sliderOpacity->setEnabled(enabled);
    ui->buttonRemove->setEnabled(enabled);
    ui->buttonCreate->setEnabled(enabled);
    ui->comboBoxCorrespondence->setEnabled(enabled);
}

void CorrespondenceEditor::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ui->sliderArticulation->setValue(0);
    ui->comboBoxCorrespondence->clear();
}

void CorrespondenceEditor::setup3DView() {
    graphicsWindow = new Qt3DExtras::Qt3DWindow();
    QWidget *container = QWidget::createWindowContainer(graphicsWindow);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);

    rootEntity = new Qt3DCore::QEntity();

    // Setup the framegraph that holds the render settings
    framegraphEntity = new Qt3DRender::QRenderSettings(rootEntity);
    framegraphEntity->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    framegraphEntity->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    rootEntity->addComponent(framegraphEntity);

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    framegraphEntity->setActiveFrameGraph(renderSurfaceSelector);
    Qt3DRender::QViewport *mainViewport = new Qt3DRender::QViewport(renderSurfaceSelector);
    mainViewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    /*
     * First branch to be executed
     */
    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(mainViewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(255, 255, 255, 255));

    /*
     * Second branch holding the viewport of the front facing view
     */
    setupCamera(leftCamera, QVector3D(0.f, 0.f, 100.f), QVector3D(0.f, 0.f, 300.f),
                mainViewport, QRectF(0.0, 0.0, 0.5, 1.0));

    /*
     * Third branch holding the viewport of the back facing view
     */
    setupCamera(rightCamera, QVector3D(0.f, 0.f, -100.f), QVector3D(0.f, 0.f, -300.f),
                mainViewport, QRectF(0.5, 0.0, 0.5, 1.0));

    resetCameras();

    // Setup scene root that is to hold all actual objects of the scene
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    graphicsWindow->setActiveFrameGraph(framegraphEntity->activeFrameGraph());
    graphicsWindow->setRootEntity(rootEntity);
}

void CorrespondenceEditor::setupCamera(Qt3DRender::QCamera *&camera,
                                       QVector3D position,
                                       QVector3D lightPosition,
                                       Qt3DRender::QViewport *mainViewport,
                                       QRectF viewportRect) {
    camera = new Qt3DRender::QCamera(rootEntity);
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.f);
    camera->setPosition(position);
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0.f, 1.f, 0.f));

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(camera);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setIntensity(0.5f);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(lightPosition);
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);

    Qt3DExtras::QOrbitCameraController *orbitController = new Qt3DExtras::QOrbitCameraController(camera);
    orbitController->setCamera(camera);
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(mainViewport);
    viewport->setNormalizedRect(viewportRect);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(camera);
}

void CorrespondenceEditor::updateCameraLenses() {
    float aspectRatio = this->width() / ((float) this->height() * 1.5f);
    leftCamera->setAspectRatio(aspectRatio);
    rightCamera->setAspectRatio(aspectRatio);
}

void CorrespondenceEditor::resetCameras() {
    leftCamera->setPosition(QVector3D(0.f, 0.f, 100.f));
    leftCamera->setViewCenter(QVector3D(0, 0, 0));
    leftCamera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    rightCamera->setPosition(QVector3D(0.f, 0.f, -100.f));
    rightCamera->setViewCenter(QVector3D(0, 0, 0));
    rightCamera->setUpVector(QVector3D(0.f, 1.f, 0.f));
    updateCameraLenses();
}

float CorrespondenceEditor::cameraFieldOfView() {
    return 50.f;
}

void CorrespondenceEditor::addCorrespondencesToComboBoxCorrespondences(const Image *image, const ObjectModel *objectModel) {
    ui->comboBoxCorrespondence->clear();
    QList<ObjectImageCorrespondence> correspondences = modelManager->getCorrespondencesForImageAndObjectModel(
                *image,
                *objectModel);
    for (ObjectImageCorrespondence &correspondence : correspondences) {
        ui->comboBoxCorrespondence->addItem(correspondence.getID());
    }
    ui->comboBoxCorrespondence->setEnabled(correspondences.size() > 0);
}

void CorrespondenceEditor::updateCurrentlyEditedCorrespondence() {
    currentCorrespondence->setPosition(ui->spinBoxTranslationX->value(),
                                       ui->spinBoxTranslationY->value(),
                                       ui->spinBoxTranslationZ->value());
    currentCorrespondence->setRotation(ui->spinBoxRotationX->value(),
                                       ui->spinBoxRotationY->value(),
                                       ui->spinBoxRotationZ->value());
    currentCorrespondence->setArticulation(ui->sliderArticulation->value());
    modelManager->updateObjectImageCorrespondence(currentCorrespondence->getID(),
                                                  currentCorrespondence->getPosition(),
                                                  currentCorrespondence->getRotation(),
                                                  currentCorrespondence->getArticulation(),
                                                  currentCorrespondence->isAccepted());
}

void CorrespondenceEditor::removeCurrentlyEditedCorrespondence() {
    modelManager->removeObjectImageCorrespondence(currentCorrespondence->getID());
}

void CorrespondenceEditor::onSpinBoxTranslationXValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxTranslationYValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxTranslationZValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxRotationXValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxRotationYValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onSpinBoxRotationZValueChanged(double value) {
    updateCurrentlyEditedCorrespondence();
}

void CorrespondenceEditor::onButtonPredictClicked() {
    // TODO: implement
}

void CorrespondenceEditor::onButtonCreateClicked() {
    emit buttonCreateClicked();
}

void CorrespondenceEditor::onComboBoxCorrespondenceIndexChanged(int index) {
    if (index < 0 || ignoreComboBoxIndexChange)
        return;

    QList<ObjectImageCorrespondence> correspondencesForImage = modelManager->getCorrespondencesForImage(*currentlySelectedImage.get());
    ObjectImageCorrespondence correspondence = correspondencesForImage.at(index);
    setCorrespondenceToEdit(&correspondence);
}

void CorrespondenceEditor::onSliderOpacityValueChanged(int value) {
    emit opacityChanged(value);
}

void CorrespondenceEditor::setObjectModelOnGraphicsWindow(const QString &objectModel) {
    if (sceneEntity != Q_NULLPTR) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
    }

    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    ObjectModelRenderable *objectModelRenderable = new ObjectModelRenderable(sceneEntity,
                                                                             objectModel,
                                                                             "");
    objectPicker = new Qt3DRender::QObjectPicker(objectModelRenderable);
    objectModelRenderable->addComponent(objectPicker);
    connect(objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(objectPickerClicked(Qt3DRender::QPickEvent*)));
}

void CorrespondenceEditor::setObjectModel(ObjectModel *objectModel) {
    if (objectModel == Q_NULLPTR) {
        qDebug() << "Object model to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    ignoreComboBoxIndexChange = true;
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence.reset();
    resetControlsValues();
    currentObjectModel.reset(new ObjectModel(*objectModel));
    setObjectModelOnGraphicsWindow(currentObjectModel->getAbsolutePath());
    addCorrespondencesToComboBoxCorrespondences(currentlySelectedImage.get(), objectModel);
    ignoreComboBoxIndexChange = false;
}

void CorrespondenceEditor::onSelectedImageChanged(int index) {
    reset();
    ui->sliderOpacity->setEnabled(true);
    Image selectedImage = modelManager->getImages().at(index);
    currentlySelectedImage.reset(new Image(selectedImage));
}

void CorrespondenceEditor::setCorrespondenceToEdit(ObjectImageCorrespondence *correspondence) {
    if (correspondence == Q_NULLPTR) {
        qDebug() << "Correspondence to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting correspondence (" + correspondence->getID() + ", " + correspondence->getImage()->getImagePath()
                + ", " + correspondence->getObjectModel()->getPath() + ") to display.";
    ignoreComboBoxIndexChange = true;
    resetControlsValues();
    currentCorrespondence.reset(new ObjectImageCorrespondence(*correspondence));
    currentObjectModel.reset(new ObjectModel(*correspondence->getObjectModel()));
    setEnabledCorrespondenceEditorControls(true);
    // Might not have been set correctly that's why we set it here
    addCorrespondencesToComboBoxCorrespondences(correspondence->getImage(), correspondence->getObjectModel());
    QVector3D position = currentCorrespondence->getPosition();
    QVector3D rotation = currentCorrespondence->getRotation();
    float articulation = currentCorrespondence->getArticulation();
    ui->spinBoxTranslationX->setValue(position.x());
    ui->spinBoxTranslationY->setValue(position.y());
    ui->spinBoxTranslationZ->setValue(position.z());
    ui->spinBoxRotationX->setValue(rotation.x());
    ui->spinBoxRotationY->setValue(rotation.y());
    ui->spinBoxRotationZ->setValue(rotation.z());
    ui->sliderArticulation->setValue(articulation);
    setObjectModelOnGraphicsWindow(currentCorrespondence->getObjectModel()->getAbsolutePath());
    ignoreComboBoxIndexChange = false;
}

void CorrespondenceEditor::visualizeLastClickedPosition(int correspondencePointIndex) {
    Q_ASSERT(correspondencePointIndex >= 0);
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(sceneEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh(sphereEntity);
    sphereMesh->setRadius(0.5f);
    sphereMesh->setRings(10);
    sphereMesh->setSlices(10);
    sphereEntity->addComponent(sphereMesh);
    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial(sphereEntity);
    phongMaterial->setAmbient(DisplayHelper::colorForCorrespondencePointIndex(correspondencePointIndex));
    sphereEntity->addComponent(phongMaterial);
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform(sphereEntity);
    sphereTransform->setTranslation(lastClickedPosition);
    sphereEntity->addComponent(sphereTransform);
    positionSpheres.append(sphereEntity);
}

void CorrespondenceEditor::removePositionVisualizations() {
    for (Qt3DCore::QEntity *entity : positionSpheres) {
        delete entity;
        entity = Q_NULLPTR;
    }
    positionSpheres.clear();
}

void CorrespondenceEditor::onCorrespondenceCreationAborted() {
    removePositionVisualizations();
    ui->buttonCreate->setEnabled(false);
}

void CorrespondenceEditor::onCorrespondencePointFinished(QVector3D point3D,
                                                         int currentNumberOfPoints,
                                                         int minimumNumberOfPoints) {
    // -1 because we need the index, and the signal is emitted by the correspondence creator
    // after adding the point thus increasing the number to 1
    visualizeLastClickedPosition(currentNumberOfPoints - 1);
    ui->buttonCreate->setEnabled(currentNumberOfPoints >= minimumNumberOfPoints);
}

void CorrespondenceEditor::reset() {
    // Deleting the scene entity before setting an object model causes the program to crash,
    // even if a null check is employed. But we can check whether the object model has been
    // set before and only then delete the scene entity.
    if (currentObjectModel) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
        positionSpheres.clear();
    }

    setEnabledAllControls(false);
    currentObjectModel.reset();
    currentCorrespondence.reset();
    currentlySelectedImage.reset();
    resetControlsValues();
}

bool CorrespondenceEditor::isDisplayingObjectModel() {
    return currentObjectModel != Q_NULLPTR;
}

void CorrespondenceEditor::resizeEvent(QResizeEvent* event) {
    updateCameraLenses();
}

void CorrespondenceEditor::objectPickerClicked(Qt3DRender::QPickEvent *pick) {
    if (pick->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    lastClickedPosition = pick->localIntersection();
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: ("
                + QString::number(lastClickedPosition.x()) + ", "
                + QString::number(lastClickedPosition.y()) + ", "
                + QString::number(lastClickedPosition.z()) + ").";
    emit objectModelClickedAt(currentObjectModel.get(), lastClickedPosition);
}
