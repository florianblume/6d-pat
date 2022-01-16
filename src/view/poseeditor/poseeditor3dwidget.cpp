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
    , m_rootEntity(new Qt3DCore::QEntity)
    , m_objectModelRoot(new Qt3DCore::QEntity)
    , m_objectModelTransform(new Qt3DCore::QTransform)
    , m_renderStateSet(new Qt3DRender::QRenderStateSet)
    , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
    , m_depthTest(new Qt3DRender::QDepthTest) {
    setRootEntity(m_rootEntity);
    m_objectModelRoot->setParent(m_rootEntity);
    m_objectModelRoot->addComponent(m_objectModelTransform);
    m_rotationHandler.setTransform(m_objectModelTransform);
    m_translationHandler.setTransform(m_objectModelTransform);

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
    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
            [this, camera](){
        m_rotationHandler.setViewMatrix(camera->viewMatrix());
        m_translationHandler.setViewMatrix(camera->viewMatrix());
    });
    connect(camera, &Qt3DRender::QCamera::projectionMatrixChanged,
            [this, camera](){
        m_rotationHandler.setProjectionMatrix(camera->projectionMatrix());
        m_translationHandler.setProjectionMatrix(camera->projectionMatrix());

    });
    connect(this, &Qt3DExtras::Qt3DWindow::widthChanged,
            [this](){
        m_rotationHandler.setWidth(this->width());
        m_translationHandler.setWidth(this->width());
        this->camera()->lens()->setPerspectiveProjection(45.0f,
                                                         this->width() / (float) this->height(),
                                                         0.1f, 1000.0f);

    });
    connect(this, &Qt3DExtras::Qt3DWindow::heightChanged,
            [this](){
        m_rotationHandler.setHeight(this->height());
        m_translationHandler.setHeight(this->height());
        this->camera()->lens()->setPerspectiveProjection(45.0f,
                                                         this->width() / (float) this->height(),
                                                         0.1f, 1000.0f);

    });

    m_lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(m_lightEntity);
    light->setColor("white");
    light->setIntensity(0.5);
    m_lightEntity->addComponent(light);
    Qt3DCore::QTransform *m_lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    m_lightTransform->setTranslation(camera->position());
    m_lightEntity->addComponent(m_lightTransform);
    // Need to keep this although we do not actually move the camera anymore
    connect(camera, &Qt3DRender::QCamera::positionChanged,
            [m_lightTransform, this](){
        m_lightTransform->setTranslation(this->camera()->position());
    });

    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    renderSettings()->pickingSettings()->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);

    m_picker = new Qt3DRender::QObjectPicker(m_rootEntity);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    m_rootEntity->addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){
        if (!m_mouseMovedOnObjectModelRenderable &&
                m_mouseDownOnObjectModelRenderable) {
            // localIntersection() does not invert the model transformations apparently
            // only the view transformations -> we have to invet them here manually
            QVector4D localIntersection = QVector4D(pickEvent->localIntersection(), 1.0);
            localIntersection = m_objectModelTransform->matrix().inverted() * localIntersection;
            Q_EMIT positionClicked(localIntersection.toVector3D());
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
}

PoseEditor3DWindow::~PoseEditor3DWindow() {
    if (objectModelRenderable) {
        objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
        objectModelRenderable->deleteLater();
    }
}

void PoseEditor3DWindow::reset3DScene() {
    // No need to call camera()->viewAll(); here since we do this when
    // loading the object model and do not transform the camera afterwards
    m_objectModelTransform->setTranslation(QVector3D());
    m_objectModelTransform->setRotation(QQuaternion());
}

void PoseEditor3DWindow::onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    m_lightEntity->setEnabled(!objectModelRenderable->hasTextureMaterial());
    camera()->viewAll();
    if (status == Qt3DRender::QSceneLoader::Ready) {
        objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings().click3DSize());
    }
}

void PoseEditor3DWindow::onObjectModelRenderablePressed(Qt3DRender::QPickEvent *event) {
    m_pressedMouseButton = event->button();
    // Set this so we know for rotation that the mouse was pressed on the renderable first
    m_mouseMovedOnObjectModelRenderable = false;
    m_mouseDownOnObjectModelRenderable = true;
    m_rotationHandler.initializeRotation(event->position());
    m_translationHandler.initializeTranslation(event->localIntersection(), event->worldIntersection());
}

void PoseEditor3DWindow::onObjectModelRenderableMoved(Qt3DRender::QPickEvent *event) {
    // We don't need to call this, mouseMoveEvent already sets the moved bool to true
    // no matter where the mouse moves on the widget
    //m_mouseMovedOnObjectModelRenderable = true;
    Q_EMIT mouseMoved(event->localIntersection());
}

void PoseEditor3DWindow::mouseReleaseEvent(QMouseEvent */*event*/) {
    m_pressedMouseButton = Qt3DRender::QPickEvent::NoButton;
}

void PoseEditor3DWindow::mouseMoveEvent(QMouseEvent *event) {
    m_mouseMovedOnObjectModelRenderable = true;
    if (m_pressedMouseButton == Qt3DRender::QPickEvent::LeftButton && m_mouseDownOnObjectModelRenderable) {
        m_translationHandler.translate(event->pos());
    } else if (m_pressedMouseButton == Qt3DRender::QPickEvent::RightButton &&  m_mouseDownOnObjectModelRenderable) {
        m_rotationHandler.rotate(event->pos());
    }
}

void PoseEditor3DWindow::wheelEvent(QWheelEvent *event) {
    m_objectModelTransform->setTranslation(m_objectModelTransform->translation()
                                           + QVector3D(0, 0, event->angleDelta().y() / 15.f));
}

void PoseEditor3DWindow::onCurrentSettingsChanged(const Settings &settings) {
    if (objectModelRenderable) {
        objectModelRenderable->setClickDiameter(settings.click3DSize());
    }
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    if (objectModelRenderable) {
        objectModelRenderable->setParent((Qt3DCore::QNode*) 0);
        delete objectModelRenderable;
    }
    objectModelRenderable = new ObjectModelRenderable(m_objectModelRoot);
    // Needs to be placed after setRootEntity on the window because it doesn't work otherwise -> leave it here
    connect(objectModelRenderable, &ObjectModelRenderable::statusChanged, this, &PoseEditor3DWindow::onObjectRenderableStatusChanged);
    objectModelRenderable->setObjectModel(objectModel);
    objectModelRenderable->setEnabled(true);
    if (m_settingsStore) {
        objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings().click3DSize());
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
