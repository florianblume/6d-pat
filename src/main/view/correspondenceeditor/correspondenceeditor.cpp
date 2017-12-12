#include "correspondenceeditor.h"
#include "ui_correspondenceeditor.h"
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

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    setupView();
}

CorrespondenceEditor::~CorrespondenceEditor()
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

void CorrespondenceEditor::setEnabledCorrespondenceEditorControls(bool enabled) {
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

void CorrespondenceEditor::setEnabledAllControls(bool enabled) {
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

void CorrespondenceEditor::resetControlsValues() {
    ui->spinBoxTranslationX->setValue(0);
    ui->spinBoxTranslationY->setValue(0);
    ui->spinBoxTranslationZ->setValue(0);
    ui->spinBoxRotationX->setValue(0);
    ui->spinBoxRotationY->setValue(0);
    ui->spinBoxRotationZ->setValue(0);
    ui->sliderArticulation->setValue(0);
}

void CorrespondenceEditor::setupView() {
    qDebug() << "Setting up 3D views.";
    setup3DWindow(leftWindow, leftRootEntity, leftSceneEntity, leftFramegraphEntity);
    // Separator between the views
    QFrame *line = new QFrame(ui->graphicsFrame);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    ui->graphicsFrame->layout()->addWidget(line);
    setup3DWindow(rightWindow, rightRootEntity, rightSceneEntity, rightFramegraphEntity);
}

void CorrespondenceEditor::setup3DWindow(WindowPointer& window,
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

void CorrespondenceEditor::updateCurrentlyEditedCorrespondence() {
    currentCorrespondence->setPosition(ui->spinBoxTranslationX->value(),
                                       ui->spinBoxTranslationY->value(),
                                       ui->spinBoxTranslationZ->value());
    currentCorrespondence->setRotation(ui->spinBoxRotationX->value(),
                                       ui->spinBoxRotationY->value(),
                                       ui->spinBoxRotationZ->value());
    currentCorrespondence->setArticulation(ui->sliderArticulation->value());
    emit correspondenceUpdated(currentCorrespondence);
}

void CorrespondenceEditor::setOpacityOfObjectModel(int opacity) {
    // TODO: implement
}

void CorrespondenceEditor::removeCurrentlyEditedCorrespondence() {
    setEnabledAllControls(false);
    resetControlsValues();
    emit correspondenceRemoved(currentCorrespondence);
    currentCorrespondence = NULL;
}

void CorrespondenceEditor::predictPositionOfObjectModels() {
    // TODO: implement
}

void CorrespondenceEditor::setObjectModelForWindow(WindowPointer window,
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

void CorrespondenceEditor::setObjectModel(const ObjectModel* objectModel) {
    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence = NULL;
    resetControlsValues();
    currentObjectModel = objectModel;
    setObjectModelForWindow(leftWindow, leftRootEntity, leftSceneEntity,
                            currentObjectModel->getAbsolutePath(), leftObjectPicker);
    setObjectModelForWindow(rightWindow, rightRootEntity, rightSceneEntity,
                            currentObjectModel->getAbsolutePath(), rightObjectPicker);
}

void CorrespondenceEditor::setCorrespondenceToEdit(ObjectImageCorrespondence* correspondence) {
    qDebug() << "Setting correspondence (" + correspondence->getID() + ", " + correspondence->getImage()->getImagePath()
                + ", " + correspondence->getObjectModel()->getPath() + ") to display.";
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
    setObjectModelForWindow(leftWindow, leftRootEntity, leftSceneEntity,
                            currentCorrespondence->getObjectModel()->getAbsolutePath(), leftObjectPicker);
    setObjectModelForWindow(rightWindow,rightRootEntity, rightSceneEntity,
                            currentCorrespondence->getObjectModel()->getAbsolutePath(), rightObjectPicker);
}

void CorrespondenceEditor::reset() {
    setEnabledAllControls(false);
    currentObjectModel = NULL;
    currentCorrespondence = NULL;
    if (leftWindow) {
        delete leftSceneEntity;
    }
    if (rightWindow) {
        delete rightSceneEntity;
    }
    resetControlsValues();
}

bool CorrespondenceEditor::isDisplayingObjectModel() {
    return currentObjectModel != NULL;
}

void CorrespondenceEditor::objectPickerClicked(Qt3DRender::QPickEvent *pick) {
    if (pick->button() != Qt::LeftButton)
        return;

    QVector3D point = QVector3D(pick->localIntersection().x(),
                                pick->localIntersection().y(),
                                pick->localIntersection().z());
    qDebug() << "Object model (" + currentObjectModel->getPath() + ") clicked at: ("
                + QString::number(point.x()) + ", "
                + QString::number(point.y()) + ", "
                + QString::number(point.z()) + ").";
    emit objectModelClickedAt(currentObjectModel, point);
}
