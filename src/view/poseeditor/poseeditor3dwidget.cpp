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
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QLayer>

PoseEditor3DWindow::PoseEditor3DWindow()
    : Qt3DExtras::Qt3DWindow()
    // Root framegraph
    , m_renderSurfaceSelector(new Qt3DRender::QRenderSurfaceSelector)
    , m_renderStateSet(new Qt3DRender::QRenderStateSet)
    , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
    , m_depthTest(new Qt3DRender::QDepthTest)
    , m_viewport(new Qt3DRender::QViewport)
    // First clear buffers
    , m_clearBuffers(new Qt3DRender::QClearBuffers)
    , m_noDraw(new Qt3DRender::QNoDraw)
    // Object branch
    , m_camera(new Qt3DRender::QCamera)
    , m_cameraSelector(new Qt3DRender::QCameraSelector)
    , m_objectModelLayerFilter(new Qt3DRender::QLayerFilter)
    , m_objectModelLayer(new Qt3DRender::QLayer)
    // Second clear buffers for depth
    , m_clearBuffers2(new Qt3DRender::QClearBuffers)
    , m_noDraw2(new Qt3DRender::QNoDraw)
    // Gizmo branch, to draw it ontop of the object
    , m_gizmoCameraSelector(new Qt3DRender::QCameraSelector)
    , m_gizmoRenderStateSet(new Qt3DRender::QRenderStateSet)
    , m_gizmoCullFace(new Qt3DRender::QCullFace)
    , m_gizmoLayerFilter(new Qt3DRender::QLayerFilter)
    , m_gizmoLayer(new Qt3DRender::QLayer)
    // Entity tree
    , m_rootEntity(new Qt3DCore::QEntity)
    , m_objectModelRoot(new Qt3DCore::QEntity)
    , m_objectModelTransform(new Qt3DCore::QTransform)
    , m_gizmo(new Qt3DGizmo) {

    // Setup framegraph
    // Root
    m_renderSurfaceSelector->setSurface(this);
    m_viewport->setParent(m_renderSurfaceSelector);

    // First branch, clear all buffers
    m_clearBuffers->setParent(m_viewport);
    m_clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    m_clearBuffers->setClearColor(Qt::white);
    m_noDraw->setParent(m_clearBuffers);

    // Second Branch, draw object model
    m_objectModelLayerFilter->setParent(m_viewport);
    m_objectModelLayerFilter->addLayer(m_objectModelLayer);
    m_objectModelLayer->setRecursive(true);
    m_cameraSelector->setParent(m_objectModelLayerFilter);
    m_cameraSelector->setCamera(m_camera);
    m_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_camera->setPosition(QVector3D(15, 10, 20));
    m_camera->setUpVector(QVector3D(0, 1, 0));
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    m_formerCameraPos = m_camera->position();

    // Third branch, clear depth buffers
    m_clearBuffers2->setParent(m_viewport);
    m_clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    m_noDraw2->setParent(m_clearBuffers2);

    // Fourth branch, draw gizmo
    m_gizmoLayerFilter->setParent(m_viewport);
    m_gizmoLayerFilter->addLayer(m_gizmoLayer);
    m_gizmoLayer->setRecursive(true);
    m_gizmoRenderStateSet->setParent(m_gizmoLayerFilter);
    m_gizmoRenderStateSet->addRenderState(m_gizmoCullFace);
    m_gizmoCullFace->setMode(Qt3DRender::QCullFace::NoCulling);
    m_gizmoCameraSelector->setParent(m_gizmoRenderStateSet);
    m_gizmoCameraSelector->setCamera(m_camera);

    setActiveFrameGraph(m_renderSurfaceSelector);
    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    renderSettings()->pickingSettings()->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);
    renderSettings()->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPriorityPick);

    setRootEntity(m_rootEntity);
    m_objectModelRoot->setParent(m_rootEntity);
    m_objectModelRoot->addComponent(m_objectModelTransform);

    // Setup gizmo
    m_gizmo->setParent(m_rootEntity);
    m_gizmo->setDelegateTransform(m_objectModelTransform);
    m_gizmo->setCamera(m_camera);
    m_gizmo->setEnabled(false);
    m_gizmo->setHideMouseWhileTransforming(false);
    m_gizmo->addComponent(m_gizmoLayer);
    m_gizmo->setScale(150);
    m_gizmo->setScaleToCameraDistance(true);

    connect(this, &Qt3DExtras::Qt3DWindow::widthChanged,
            [this](){
        m_gizmo->setWindowSize(size());
        m_camera->lens()->setPerspectiveProjection(
                    45.0f,
                    this->width() / (float) this->height(),
                    0.1f, 1000.0f);

    });
    connect(this, &Qt3DExtras::Qt3DWindow::heightChanged,
            [this](){
        m_gizmo->setWindowSize(size());
        m_camera->lens()->setPerspectiveProjection(
                    45.0f,
                    this->width() / (float) this->height(),
                    0.1f, 1000.0f);

    });

    // Setup some light
    m_lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(m_lightEntity);
    light->setColor("white");
    light->setIntensity(0.5);
    m_lightEntity->addComponent(light);
    m_lightEntity->addComponent(m_objectModelLayer);
    Qt3DCore::QTransform *m_lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    //m_lightTransform->setTranslation(2 * m_camera->position());
    m_lightEntity->addComponent(m_lightTransform);
    // Need to keep this although we do not actually move the camera anymore
    connect(m_camera, &Qt3DRender::QCamera::positionChanged,
            [m_lightTransform, this](){
        m_lightTransform->setTranslation(2 * m_camera->position());
    });

    // Setup the picker for the object model
    m_picker = new Qt3DRender::QObjectPicker(m_objectModelRoot);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    m_objectModelRoot->addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){
        if (pickEvent->button() == m_settingsStore->currentSettings()->addCorrespondencePointMouseButton()) {
            // localIntersection() does not invert the model transformations apparently
            // only the view transformations -> we have to invet them here manually
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
}

PoseEditor3DWindow::~PoseEditor3DWindow() {
    if (m_objectModelRenderable) {
        m_objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
        m_objectModelRenderable->deleteLater();
    }
}

void PoseEditor3DWindow::reset3DScene() {
    // No need to call camera()->viewAll(); here since we do this when
    // loading the object model and do not transform the camera afterwards
    m_objectModelTransform->setTranslation(QVector3D());
    m_objectModelTransform->setRotation(QQuaternion());
}

void PoseEditor3DWindow::showGizmo(bool show) {
    if (m_objectModelRenderable) {
        m_gizmo->setEnabled(show);
    }
    m_showGizmo = show;
}

void PoseEditor3DWindow::onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    // Only enable when we don't have texture, texture is enough to be able to properly see the
    // model and lightning makes it more difficult
    m_lightEntity->setEnabled(!m_objectModelRenderable->hasTextureMaterial());
    m_camera->viewEntity(m_objectModelRenderable);
    if (status == Qt3DRender::QSceneLoader::Ready) {
        m_objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings()->click3DSize());
    }
}

void PoseEditor3DWindow::onObjectModelRenderablePressed(Qt3DRender::QPickEvent *event) {
    m_pressedMouseButton = event->button();
    // Set this so we know for rotation that the mouse was pressed on the renderable first
    m_mouseMovedOnObjectModelRenderable = false;
    m_mouseDownOnObjectModelRenderable = true;
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

void PoseEditor3DWindow::wheelEvent(QWheelEvent *event) {
    float delta = -event->angleDelta().y() / 1000.f;
    m_camera->setPosition(m_camera->position() + m_camera->position() * delta);
}

bool PoseEditor3DWindow::showingGizmo() {
    return m_showGizmo;
}

void PoseEditor3DWindow::onCurrentSettingsChanged(SettingsPtr settings) {
    if (m_objectModelRenderable) {
        m_objectModelRenderable->setClickDiameter(settings->click3DSize());
    }
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    if (m_objectModelRenderable) {
        m_objectModelRenderable->setParent((Qt3DCore::QNode*) 0);
        delete m_objectModelRenderable;
    }
    m_formerCameraPos = m_camera->position();
    m_objectModelRenderable = new ObjectModelRenderable(m_objectModelRoot);
    m_objectModelRenderable->addComponent(m_objectModelLayer);
    // Needs to be placed after setRootEntity on the window because it doesn't work otherwise -> leave it here
    connect(m_objectModelRenderable, &ObjectModelRenderable::statusChanged,
            this, &PoseEditor3DWindow::onObjectRenderableStatusChanged);
    m_objectModelRenderable->setObjectModel(objectModel);
    m_objectModelRenderable->setEnabled(true);
    if (m_showGizmo) {
        m_gizmo->setEnabled(true);
    }
    if (m_settingsStore) {
        m_objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings()->click3DSize());
    }
    setClicks({});
}

void PoseEditor3DWindow::setClicks(const QList<QVector3D> &clicks) {
    if (m_objectModelRenderable) {
        m_objectModelRenderable->setClicks(clicks);
    }
}

void PoseEditor3DWindow::reset() {
    setClicks({});
    if (m_objectModelRenderable) {
        m_objectModelRenderable->setEnabled(false);
        m_gizmo->setEnabled(false);
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
