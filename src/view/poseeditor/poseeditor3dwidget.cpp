#include "view/poseeditor/poseeditor3dwidget.hpp"
#include "misc/global.hpp"

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
    , m_renderStateSet(new Qt3DRender::QRenderStateSet)
    , m_multisampleAntialiasing(new Qt3DRender::QMultiSampleAntiAliasing)
    , m_depthTest(new Qt3DRender::QDepthTest) {
    m_rootEntity = new Qt3DCore::QEntity();
    setRootEntity(m_rootEntity);

    m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    m_renderStateSet->addRenderState(m_depthTest);
    m_renderStateSet->addRenderState(m_multisampleAntialiasing);
    activeFrameGraph()->setParent(m_renderStateSet);
    setActiveFrameGraph(m_renderStateSet);

    Qt3DRender::QCamera *cameraEntity = this->camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    m_cameraController = new Qt3DExtras::QOrbitCameraController(m_rootEntity);
    m_cameraController->setCamera(camera());

    m_lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(m_lightEntity);
    light->setColor("white");
    light->setIntensity(0.5);
    m_lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    m_lightEntity->addComponent(lightTransform);
    connect(camera(), &Qt3DRender::QCamera::positionChanged,
            [lightTransform, this](){lightTransform->setTranslation(this->camera()->position());});

    this->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    m_picker = new Qt3DRender::QObjectPicker(m_rootEntity);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    m_rootEntity->addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){
        if (!m_mouseMoved) {
            Q_EMIT positionClicked(pickEvent->localIntersection());
        }
        m_mouseMoved = false;
        m_mouseDown = false;
    });
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &PoseEditor3DWindow::onPoseRenderableMoved);

}

PoseEditor3DWindow::~PoseEditor3DWindow() {
    if (objectModelRenderable) {
        objectModelRenderable->setParent((Qt3DCore::QNode *) 0);
        objectModelRenderable->deleteLater();
    }
}

void PoseEditor3DWindow::onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    m_lightEntity->setEnabled(!objectModelRenderable->hasTextureMaterial());
    camera()->viewAll();
    if (status == Qt3DRender::QSceneLoader::Ready) {
        objectModelRenderable->setClickDiameter(m_settingsStore->currentSettings()->click3DSize());
    }
}

void PoseEditor3DWindow::onPoseRenderableMoved() {
    if (m_mouseDown) {
        m_mouseMoved = true;
    }
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

void PoseEditor3DWindow::mousePressEvent(QMouseEvent *e) {
    m_mouseDown = true;
}
