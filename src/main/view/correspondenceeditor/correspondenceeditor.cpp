#include "correspondenceeditor.hpp"
#include "ui_correspondenceeditor.h"
#include "view/rendering/objectmodelrenderable.hpp"

#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <QUrl>
#include <QThread>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    setup3DView();
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    if (graphicsWindow) {
        graphicsWindow->destroy();
        delete graphicsWindow;
    }
    delete ui;
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
    ui->buttonRemove->setEnabled(enabled);
}

void CorrespondenceEditor::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ui->sliderArticulation->setValue(0);
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
    camera->setNearPlane(0.001f);
    camera->setFarPlane(5000.f);
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

void CorrespondenceEditor::updateCurrentlyEditedCorrespondence() {
    currentCorrespondence->setPosition(ui->spinBoxTranslationX->value(),
                                       ui->spinBoxTranslationY->value(),
                                       ui->spinBoxTranslationZ->value());
    currentCorrespondence->setRotation(ui->spinBoxRotationX->value(),
                                       ui->spinBoxRotationY->value(),
                                       ui->spinBoxRotationZ->value());
    currentCorrespondence->setArticulation(ui->sliderArticulation->value());
    emit correspondenceUpdated(currentCorrespondence.get());
}

void CorrespondenceEditor::setOpacityOfObjectModel(int opacity) {
    // TODO: implement
}

void CorrespondenceEditor::removeCurrentlyEditedCorrespondence() {
    setEnabledAllControls(false);
    resetControlsValues();
    emit correspondenceRemoved(currentCorrespondence.get());
    currentCorrespondence.reset();
}

void CorrespondenceEditor::predictPositionOfObjectModels() {
    // TODO: implement
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
    //leftCamera->viewEntity(objectModelRenderable);
    //rightCamera->viewEntity(objectModelRenderable);

    // Picker to enable the user to select points on the object

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
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence.reset();
    resetControlsValues();
    currentObjectModel.reset(new ObjectModel(*objectModel));
    setObjectModelOnGraphicsWindow(currentObjectModel->getAbsolutePath());
    //resetCameras();
}

void CorrespondenceEditor::setCorrespondenceToEdit(ObjectImageCorrespondence *correspondence) {
    if (correspondence == Q_NULLPTR) {
        qDebug() << "Correspondence to set was null. Restting view.";
        reset();
        return;
    }

    qDebug() << "Setting correspondence (" + correspondence->getID() + ", " + correspondence->getImage()->getImagePath()
                + ", " + correspondence->getObjectModel()->getPath() + ") to display.";
    currentCorrespondence.reset(new ObjectImageCorrespondence(*correspondence));
    setEnabledCorrespondenceEditorControls(true);
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
}

void CorrespondenceEditor::reset() {
    // Deleting the scene entity before setting an object model causes the program to crash,
    // even if a null check is employed. But we can check whether the object model has been
    // set before and only then delete the scene entity.
    if (currentObjectModel) {
        delete sceneEntity;
        sceneEntity = Q_NULLPTR;
    }

    setEnabledAllControls(false);
    currentObjectModel.reset();
    currentCorrespondence.reset();
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

    QVector3D point = pick->localIntersection();
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: ("
                + QString::number(point.x()) + ", "
                + QString::number(point.y()) + ", "
                + QString::number(point.z()) + ").";
    emit objectModelClickedAt(currentObjectModel.get(), point);
}
