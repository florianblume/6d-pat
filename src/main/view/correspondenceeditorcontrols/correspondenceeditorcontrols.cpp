#include "correspondenceeditorcontrols.h"
#include "ui_correspondenceeditorcontrols.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QDepthTest>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QRenderSettings>
#include <QUrl>

CorrespondenceEditorControls::CorrespondenceEditorControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditorControls)
{
    ui->setupUi(this);
    setupView();
}

CorrespondenceEditorControls::~CorrespondenceEditorControls()
{
    if (leftWindow)
        leftWindow->destroy();
    if (rightWindow)
        rightWindow->destroy();
    if (leftObjectPicker)
        delete leftObjectPicker;
    if (rightObjectPicker)
        delete rightObjectPicker;
    delete leftWindow;
    delete rightWindow;
    delete ui;
}

void CorrespondenceEditorControls::setEnabledCorrespondenceEditorControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->sliderArticulation->setEnabled(enabled);
    ui->sliderOpacity->setEnabled(enabled);
    // The next line is the difference to setEnabledAllControls
    ui->buttonPredict->setEnabled(!enabled);
    ui->buttonRemove->setEnabled(enabled);
}

void CorrespondenceEditorControls::setEnabledAllControls(bool enabled) {
    ui->spinBoxTranslationX->setEnabled(enabled);
    ui->spinBoxTranslationY->setEnabled(enabled);
    ui->spinBoxTranslationZ->setEnabled(enabled);
    ui->spinBoxRotationX->setEnabled(enabled);
    ui->spinBoxRotationY->setEnabled(enabled);
    ui->spinBoxRotationZ->setEnabled(enabled);
    ui->sliderArticulation->setEnabled(enabled);
    ui->sliderOpacity->setEnabled(enabled);
    ui->buttonPredict->setEnabled(enabled);
    ui->buttonRemove->setEnabled(enabled);
}

void CorrespondenceEditorControls::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ui->sliderArticulation->setValue(0);
}

void CorrespondenceEditorControls::setupView() {
    setup3DWindow(leftWindow, leftRootEntity, leftSceneEntity, leftFramegraphEntity);
    // Separator between the views
    QFrame *line = new QFrame(ui->graphicsFrame);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    ui->graphicsFrame->layout()->addWidget(line);
    setup3DWindow(rightWindow, rightRootEntity, rightSceneEntity, rightFramegraphEntity);
}

void CorrespondenceEditorControls::setup3DWindow(WindowPointer& window,
                                                 EntityPointer& rootEntity,
                                                 EntityPointer& sceneEntity,
                                                 RenderSettingsPointer& framegraphEntity) {
    window = new Qt3DExtras::Qt3DWindow;
    QWidget *container = QWidget::createWindowContainer(window);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);

    rootEntity = new Qt3DCore::QEntity();
    // Setup the framegraph that holds the render settings
    framegraphEntity = new Qt3DRender::QRenderSettings();
    framegraphEntity->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    rootEntity->addComponent(framegraphEntity);
    framegraphEntity->setActiveFrameGraph(window->activeFrameGraph());

    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    window->setRootEntity(rootEntity);
}

void CorrespondenceEditorControls::updateCurrentlyEditedCorrespondence() {
    currentCorrespondence->setPosition(ui->spinBoxTranslationX->value(),
                                       ui->spinBoxTranslationY->value(),
                                       ui->spinBoxTranslationZ->value());
    currentCorrespondence->setRotation(ui->spinBoxRotationX->value(),
                                       ui->spinBoxRotationY->value(),
                                       ui->spinBoxRotationZ->value());
    currentCorrespondence->setArticulation(ui->sliderArticulation->value());
    emit correspondenceUpdated(currentCorrespondence);
}

void CorrespondenceEditorControls::setOpacityOfObjectModel(int opacity) {
    // TODO: implement
}

void CorrespondenceEditorControls::removeCurrentlyEditedCorrespondence() {
    setEnabledAllControls(false);
    resetControlsValues();
    emit correspondenceRemoved(currentCorrespondence);
    currentCorrespondence = NULL;
}

void CorrespondenceEditorControls::predictPositionOfObjectModels() {
    // TODO: implement
}

void CorrespondenceEditorControls::setObjectModelForWindow(WindowPointer window,
                                                           EntityPointer rootEntity,
                                                           EntityPointer& sceneEntity,
                                                           const QString &objectModel,
                                                           ObjectPickerPointer &picker) {
    // TODO: add material
    // TODO: make initial rotation settable

    delete sceneEntity;
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    ObjectModelRenderable *objectModelRenderable = new ObjectModelRenderable(sceneEntity,
                                                                             objectModel,
                                                                             "");

    // Camera re-initialization
    Qt3DRender::QCamera *camera = window->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 100.f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Orbit controller
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(objectModelRenderable);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    // Picker to enable the user to select points on the object
    delete picker;
    picker = new Qt3DRender::QObjectPicker(objectModelRenderable);
    objectModelRenderable->addComponent(picker);
    connect(picker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(objectPickerClicked(Qt3DRender::QPickEvent*)));
}

void CorrespondenceEditorControls::setObjectModel(const ObjectModel* objectModel) {
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence = NULL;
    resetControlsValues();
    currentObjectModel = objectModel;
    setObjectModelForWindow(leftWindow, leftRootEntity, leftSceneEntity, currentObjectModel->getAbsolutePath(), leftObjectPicker);
    setObjectModelForWindow(rightWindow, rightRootEntity, rightSceneEntity, currentObjectModel->getAbsolutePath(), rightObjectPicker);
}

void CorrespondenceEditorControls::setCorrespondenceToEdit(ObjectImageCorrespondence* correspondence) {
    currentCorrespondence = correspondence;
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
    setObjectModelForWindow(leftWindow, leftRootEntity, leftSceneEntity, currentCorrespondence->getObjectModel()->getAbsolutePath(), leftObjectPicker);
    setObjectModelForWindow(rightWindow,rightRootEntity, rightSceneEntity, currentCorrespondence->getObjectModel()->getAbsolutePath(), rightObjectPicker);
}

void CorrespondenceEditorControls::reset() {
    return;
    setEnabledAllControls(false);
    currentObjectModel = NULL;
    currentCorrespondence = NULL;
    if (leftWindow) {
        delete leftSceneEntity;
        leftSceneEntity = new Qt3DCore::QEntity(leftRootEntity);
        leftWindow->setRootEntity(leftRootEntity);
    }
    if (rightWindow) {
        delete rightSceneEntity;
    }
    resetControlsValues();
}

bool CorrespondenceEditorControls::isDisplayingObjectModel() {
    return currentObjectModel != NULL;
}

void CorrespondenceEditorControls::objectPickerClicked(Qt3DRender::QPickEvent *pick) {
    emit objectModelClickedAt(currentObjectModel,
                              QVector3D(pick->localIntersection().x(),
                                        pick->localIntersection().y(),
                                        pick->localIntersection().z()));
}
