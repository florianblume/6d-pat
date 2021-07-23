#include "view/poseeditor/poseeditor3dwidget.hpp"
#include "misc/global.hpp"
#include "view/misc/displayhelper.hpp"

#include <QVector3D>
#include <QUrl>
#include <QTimer>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/qpickingsettings.h>

PoseEditor3DWindow::PoseEditor3DWindow()
    : Qt3DExtras::Qt3DWindow()
    , m_objectModelTransform(new Qt3DCore::QTransform)
    , m_renderStateSet(new Qt3DRender::QRenderStateSet)
    , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
    , m_depthTest(new Qt3DRender::QDepthTest)
    , m_mouseDevice(new Qt3DInput::QMouseDevice)
    , m_mouseHandler(new Qt3DInput::QMouseHandler) {
    m_rootEntity = new Qt3DCore::QEntity();
    setRootEntity(m_rootEntity);
    m_rootEntity->addComponent(m_objectModelTransform);

    m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    m_renderStateSet->addRenderState(m_depthTest);
    m_renderStateSet->addRenderState(m_multisampleAntialiasing);
    activeFrameGraph()->setParent(m_renderStateSet);
    setActiveFrameGraph(m_renderStateSet);

    Qt3DRender::QCamera *camera = this->camera();

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    m_lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(m_lightEntity);
    light->setColor("white");
    light->setIntensity(0.5);
    m_lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    lightTransform->setTranslation(camera->position());
    m_lightEntity->addComponent(lightTransform);
    connect(camera, &Qt3DRender::QCamera::positionChanged,
            [lightTransform, this](){lightTransform->setTranslation(this->camera()->position());});

    this->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    m_picker = new Qt3DRender::QObjectPicker(m_rootEntity);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    m_rootEntity->addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){
        if (!m_mouseMovedOnObjectModelRenderable &&
                m_mouseDownOnObjectModelRenderable) {
            Q_EMIT positionClicked(pickEvent->localIntersection());
        }
        m_mouseMovedOnObjectModelRenderable = false;
        m_mouseDownOnObjectModelRenderable = false;
    });
    connect(m_picker, &Qt3DRender::QObjectPicker::pressed,
            this, &PoseEditor3DWindow::onObjectModelRenderablePressed);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &PoseEditor3DWindow::onObjectModelRenderableMoved);
    connect(m_picker, &Qt3DRender::QObjectPicker::exited,
            this, &PoseEditor3DWindow::mouseExited);

    m_rootEntity->addComponent(m_mouseHandler);
    connect(m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
            this, &PoseEditor3DWindow::onMousePositionChanged);
    connect(m_mouseHandler, &Qt3DInput::QMouseHandler::wheel,
            this, &PoseEditor3DWindow::onMouseWheel);
    m_mouseDevice->setParent(m_rootEntity);
    m_mouseHandler->setSourceDevice(m_mouseDevice);
}

PoseEditor3DWindow::~PoseEditor3DWindow() {
    if (objectModelRenderable) {
        objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
        objectModelRenderable->deleteLater();
    }
}

void PoseEditor3DWindow::reset3DScene() {
    Qt3DRender::QCamera *camera = this->camera();
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->viewAll();
}

void PoseEditor3DWindow::onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    m_lightEntity->setEnabled(!objectModelRenderable->hasTextureMaterial());
    camera()->viewAll();
    if (status == Qt3DRender::QSceneLoader::Ready) {
        objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings()->click3DSize());
    }
}

void PoseEditor3DWindow::onObjectModelRenderablePressed(Qt3DRender::QPickEvent *event) {
    // Set this so we know for rotation that the mouse was pressed on the renderable first
    m_mouseMovedOnObjectModelRenderable = false;
    m_mouseDownOnObjectModelRenderable = true;
    // We need the world intersection to calculate the absolute difference
    m_translationStartVector = event->worldIntersection();
    // Reset the translation difference to start new
    m_translationDifference = QVector3D(0, 0, 0);
    m_initialPosition = m_objectModelTransform->translation();
    QVector3D pointOnModel = event->localIntersection();
    // Project the point on the model using its transform, the view and the projection matrix
    // to obtain the depth at that mouse position
    QVector3D projected = pointOnModel.project(camera()->viewMatrix() * m_objectModelTransform->matrix(),
                                               camera()->projectionMatrix(),
                                               QRect(0, 0, width(), height()));
    // Store the depth for the mouseMove event
    // TODO do we need to make that configurable if the camera is somehow rotated?
    m_depth = projected.z();
}

void PoseEditor3DWindow::onObjectModelRenderableReleased(Qt3DRender::QPickEvent *event) {
    m_mouseDownOnObjectModelRenderable = false;
    m_mouseMovedOnObjectModelRenderable = false;
}

void PoseEditor3DWindow::onObjectModelRenderableMoved(Qt3DRender::QPickEvent *event) {
    m_mouseMovedOnObjectModelRenderable = true;
    Q_EMIT mouseMoved(event->localIntersection());
}

void PoseEditor3DWindow::onMousePositionChanged(Qt3DInput::QMouseEvent *mouse) {
    /*
    m_arcBallEndVector = DisplayHelper::arcBallVectorForMousePos(QPoint(mouse->x(), mouse->y()), size());

    QVector3D direction = m_arcBallEndVector - m_arcBallStartVector;
    QVector3D rotationAxis = QVector3D(-direction.y(), direction.x(), 0.0).normalized();
    float angle = (float)qRadiansToDegrees(direction.length() * 3.141593);

    QMatrix4x4 addRotation;
    addRotation.rotate(angle, rotationAxis.x(), rotationAxis.y(), rotationAxis.z());
    QMatrix4x4 rotation = m_selectedPoseRenderable->transform()->matrix();
    rotation = addRotation * rotation;
    // Restore position of before rotation
    rotation.setColumn(3, QVector4D(m_selectedPoseRenderable->transform()->translation(), 1.0));
    m_selectedPoseRenderable->transform()->setMatrix(rotation);
    m_selectedPose->setRotation(m_selectedPoseRenderable->transform()->rotation().toRotationMatrix());

    m_arcBallStartVector = m_arcBallEndVector;
    m_poseRenderableRotated = true;
    */
}

void PoseEditor3DWindow::onMouseWheel(Qt3DInput::QWheelEvent *wheel) {
    m_objectModelTransform->setTranslation(m_objectModelTransform->translation() + QVector3D(0, 0, wheel->angleDelta().y() / 15.f));
}

void PoseEditor3DWindow::onCurrentSettingsChanged(SettingsPtr settings) {
    if (objectModelRenderable) {
        objectModelRenderable->setClickDiameter(settings->click3DSize());
    }
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    if (objectModelRenderable) {
        objectModelRenderable->setParent((Qt3DCore::QNode*) 0);
        delete objectModelRenderable;
    }
    objectModelRenderable = new ObjectModelRenderable(m_rootEntity);
    // Needs to be placed after setRootEntity on the window because it doesn't work otherwise -> leave it here
    connect(objectModelRenderable, &ObjectModelRenderable::statusChanged, this, &PoseEditor3DWindow::onObjectRenderableStatusChanged);
    objectModelRenderable->setObjectModel(objectModel);
    objectModelRenderable->setEnabled(true);
    if (m_settingsStore) {
        objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings()->click3DSize());
    }
    setClicks({});
}

void PoseEditor3DWindow::setClicks(const QList<QVector3D> &clicks) {
    if (objectModelRenderable) {
        objectModelRenderable->setClicks(clicks);
    }
}

void PoseEditor3DWindow::reset() {
    setClicks({});
    if (objectModelRenderable) {
        objectModelRenderable->setEnabled(false);
    }
}

void PoseEditor3DWindow::setSettingsStore(SettingsStore *settingsStore) {
    Q_ASSERT(settingsStore);
    if (!this->m_settingsStore) {
        disconnect(settingsStore, &SettingsStore::currentSettingsChanged,
                   this, &PoseEditor3DWindow::onCurrentSettingsChanged);
    }
    this->m_settingsStore = settingsStore;
    connect(settingsStore, &SettingsStore::currentSettingsChanged,
            this, &PoseEditor3DWindow::onCurrentSettingsChanged);
}
