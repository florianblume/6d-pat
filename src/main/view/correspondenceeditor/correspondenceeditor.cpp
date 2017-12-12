#include "correspondenceeditor.h"
#include "ui_correspondenceeditor.h"
#include "view/rendering/objectmodelrenderable.h"
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <QUrl>

CorrespondenceEditor::CorrespondenceEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CorrespondenceEditor)
{
    ui->setupUi(this);
    setup3DView();
}

CorrespondenceEditor::~CorrespondenceEditor()
{
    if (graphicsWindow)
        graphicsWindow->destroy();
    delete graphicsWindow;
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

void CorrespondenceEditor::setup3DView() {
    graphicsWindow = new Qt3DExtras::Qt3DWindow;
    QWidget *container = QWidget::createWindowContainer(graphicsWindow);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);

    rootEntity = new Qt3DCore::QEntity();

    // Setup the framegraph that holds the render settings
    framegraphEntity = new Qt3DRender::QRenderSettings(rootEntity);
    //framegraphEntity->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    rootEntity->addComponent(framegraphEntity);

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    framegraphEntity->setActiveFrameGraph(renderSurfaceSelector);
    Qt3DRender::QViewport *mainViewport = new Qt3DRender::QViewport(renderSurfaceSelector);
    mainViewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    // First branch to be executed
    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(mainViewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(1.0, 0.0, 0.0, 1.0));

    // Second branch holding the viewport of the front facing view
    leftCamera = new Qt3DRender::QCamera(rootEntity);
    leftCamera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    leftCamera->setPosition(QVector3D(0.f, 0.f, 100.f));
    leftCamera->setViewCenter(QVector3D(0, 0, 0));
    Qt3DRender::QViewport *leftViewport = new Qt3DRender::QViewport(mainViewport);
    leftViewport->setNormalizedRect(QRectF(0.0, 0.0, 0.5, 1.0));
    Qt3DRender::QCameraSelector *leftCameraSelector = new Qt3DRender::QCameraSelector(leftViewport);
    leftCameraSelector->setCamera(leftCamera);

    // Third branch holding the viewport of the back facing view
    rightCamera = new Qt3DRender::QCamera(rootEntity);
    rightCamera->lens()->setPerspectiveProjection(45.0f, 1.f, 0.1f, 1000.0f);
    rightCamera->setPosition(QVector3D(0.f, 0.f, -100.f));
    rightCamera->setViewCenter(QVector3D(0, 0, 0));
    Qt3DRender::QViewport *rightViewport = new Qt3DRender::QViewport(mainViewport);
    rightViewport->setNormalizedRect(QRectF(0.5, 0.0, 0.5, 1.0));
    Qt3DRender::QCameraSelector *rightCameraSelector = new Qt3DRender::QCameraSelector(rightViewport);
    rightCameraSelector->setCamera(rightCamera);

    // Setup scene root that is to hold all actual objects of the scene
    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    graphicsWindow->setActiveFrameGraph(framegraphEntity->activeFrameGraph());
    graphicsWindow->setRootEntity(rootEntity);
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

void CorrespondenceEditor::setObjectModelOnGraphicsWindow(const QString &objectModel) {
    if (sceneEntity)
        delete sceneEntity;

    sceneEntity = new Qt3DCore::QEntity(rootEntity);
    ObjectModelRenderable *objectModelRenderable = new ObjectModelRenderable(sceneEntity,
                                                                             objectModel,
                                                                             "");

    // Picker to enable the user to select points on the object

    objectPicker = new Qt3DRender::QObjectPicker(objectModelRenderable);
    objectModelRenderable->addComponent(objectPicker);
    connect(objectPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(objectPickerClicked(Qt3DRender::QPickEvent*)));
}

void CorrespondenceEditor::setObjectModel(const ObjectModel* objectModel) {
    qDebug() << "Setting object model (" + objectModel->getPath() + ") to display.";
    setEnabledCorrespondenceEditorControls(false);
    currentCorrespondence = NULL;
    resetControlsValues();
    currentObjectModel = objectModel;
    setObjectModelOnGraphicsWindow(currentObjectModel->getAbsolutePath());
    setObjectModelOnGraphicsWindow(currentObjectModel->getAbsolutePath());
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
    setObjectModelOnGraphicsWindow(currentCorrespondence->getObjectModel()->getAbsolutePath());
    setObjectModelOnGraphicsWindow(currentCorrespondence->getObjectModel()->getAbsolutePath());
}

void CorrespondenceEditor::reset() {
    setEnabledAllControls(false);
    currentObjectModel = NULL;
    currentCorrespondence = NULL;
    if (graphicsWindow) {
        delete sceneEntity;
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
