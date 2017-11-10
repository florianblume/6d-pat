#include "correspondenceeditorcontrols.h"
#include "ui_correspondenceeditorcontrols.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DExtras/QOrbitCameraController>
#include <QUrl>

CorrespondenceEditorControls::CorrespondenceEditorControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditorControls),
    leftWindow(new Qt3DExtras::Qt3DWindow),
    rightWindow(new Qt3DExtras::Qt3DWindow)
{
    ui->setupUi(this);
    setup3DWindow(leftWindow);
    QFrame *line = new QFrame(ui->graphicsFrame);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    ui->graphicsFrame->layout()->addWidget(line);
    setup3DWindow(rightWindow);
}

CorrespondenceEditorControls::~CorrespondenceEditorControls()
{
    leftWindow->destroy();
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

void CorrespondenceEditorControls::setup3DWindow(WindowPointer window) {
    QWidget *container = QWidget::createWindowContainer(window);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);
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
                                                           const ObjectModel *objectModel,
                                                           ObjectPickerPointer &picker) {
    // TODO: add material
    // TODO: make initial rotation settable
    ObjectModelRenderable *objectModelRenderable = new ObjectModelRenderable(0, objectModel->getAbsolutePath(), "");
    window->setRootEntity(objectModelRenderable);
    Qt3DRender::QCamera *camera = window->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.f, 0.f, 100.f));
    camera->setViewCenter(QVector3D(0, 0, 0));
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(objectModelRenderable);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);
    picker = new Qt3DRender::QObjectPicker(objectModelRenderable);
    objectModelRenderable->addComponent(picker);
    connect(picker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(objectPickerClicked(Qt3DRender::QPickEvent*)));
}

void CorrespondenceEditorControls::setObjectModel(const ObjectModel* objectModel) {
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence = NULL;
    resetControlsValues();
    currentObjectModel = objectModel;
    setObjectModelForWindow(leftWindow, currentObjectModel, leftObjectPicker);
    setObjectModelForWindow(rightWindow, currentObjectModel, rightObjectPicker);
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
    setObjectModelForWindow(leftWindow, currentCorrespondence->getObjectModel(), leftObjectPicker);
    setObjectModelForWindow(rightWindow, currentCorrespondence->getObjectModel(), rightObjectPicker);
}

void CorrespondenceEditorControls::reset() {
    setEnabledAllControls(false);
    currentObjectModel = NULL;
    currentCorrespondence = NULL;
    leftWindow->setRootEntity(0);
    rightWindow->setRootEntity(0);
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
