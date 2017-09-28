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
    setup3DWindow(rightWindow);
}

void CorrespondenceEditorControls::setup3DWindow(Qt3DExtras::Qt3DWindow *window) {
    QWidget *container = QWidget::createWindowContainer(window);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->graphicsFrame->layout()->addWidget(container);
}

CorrespondenceEditorControls::~CorrespondenceEditorControls()
{
    delete ui;
    if (leftWindow)
        delete leftWindow;
    if (rightWindow)
        delete rightWindow;
}

void CorrespondenceEditorControls::setModelManager(ModelManager *modelManager) {
    this->modelManager = modelManager;
}

void CorrespondenceEditorControls::setObjectModelForWindow(Qt3DExtras::Qt3DWindow *window, const ObjectModel *objectModel) {
    // TODO: add material
    // TODO: make initial rotation settable
    ObjectModelRenderable *objectModelRenderable = new ObjectModelRenderable(0, objectModel->getAbsolutePath(), "");
    window->setRootEntity(objectModelRenderable);
    Qt3DRender::QCamera *camera = window->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(objectModelRenderable);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);
}

void CorrespondenceEditorControls::setObjectModel(int index) {
    currentObjectModelIndex = index;
    if (!modelManager)
        return;

    const ObjectModel *objectModel = modelManager->getObjectModel(index);
    setObjectModelForWindow(leftWindow, objectModel);
    setObjectModelForWindow(rightWindow, objectModel);
}

void CorrespondenceEditorControls::reset() {
    Qt3DCore::QEntity *node = new Qt3DCore::QEntity();
    leftWindow->setRootEntity(node);
    rightWindow->setRootEntity(node);
}
