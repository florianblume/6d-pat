#include "view/poseeditor/poseeditor3dwindow.hpp"
#include "misc/global.hpp"

#include <QVector3D>
#include <QUrl>

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
    : Qt3DExtras::Qt3DWindow(),
      rootEntity(new Qt3DCore::QEntity) {
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    setRootEntity(rootEntity);

    Qt3DRender::QCamera *cameraEntity = this->camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    cameraController->setCamera(camera());

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);
    connect(camera(), &Qt3DRender::QCamera::positionChanged, [lightTransform, this](){lightTransform->setTranslation(this->camera()->position());});

    this->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    picker = new Qt3DRender::QObjectPicker(rootEntity);
    rootEntity->addComponent(picker);
    connect(picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){Q_EMIT positionClicked(pickEvent->localIntersection());});
    // Needs to be placed after setRootEntity on the window because it doesn't work otherwise -> leave it here
    sceneLoader = new Qt3DRender::QSceneLoader(rootEntity);
    rootEntity->addComponent(sceneLoader);
    connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &PoseEditor3DWindow::onSceneLoaderStatusChanged);

}

PoseEditor3DWindow::~PoseEditor3DWindow() {
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    sceneLoader->setSource(QUrl::fromLocalFile(objectModel.getAbsolutePath()));
}

void PoseEditor3DWindow::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        camera()->viewAll();
        // TODO: This is super ugly
        Qt3DCore::QEntity *entity = sceneLoader->entities()[0];
        Qt3DCore::QNodeVector entites = entity->childNodes();
        for (Qt3DCore::QNode *node : entites) {
            Qt3DCore::QNodeVector entities2 = node->childNodes();
            for (Qt3DCore::QNode *node : entities2) {
                if (Qt3DExtras::QPhongMaterial * v = dynamic_cast<Qt3DExtras::QPhongMaterial *>(node)) {
                    v->setAmbient(QColor(150, 150, 150));
                    v->setDiffuse(QColor(130, 130, 130));
                }
            }
        }
    }
}

void PoseEditor3DWindow::addClick(QVector3D position, QColor color) {
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh(sphereEntity);
    sphereMesh->setRings(30);
    sphereMesh->setSlices(30);
    sphereMesh->setRadius(0.1);
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform(sphereEntity);
    sphereTransform->setTranslation(position);
    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial(sphereEntity);
    sphereMaterial->setAmbient(color);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(sphereMaterial);
    clickSpheres.append(sphereEntity);
}

void PoseEditor3DWindow::removeClicks() {
    for (Qt3DCore::QEntity *sphereEntity : clickSpheres) {
        Qt3DCore::QNode *nullParent = Q_NULLPTR;
        sphereEntity->setParent(nullParent);
        sphereEntity->deleteLater();
    }
    clickSpheres.clear();
}

void PoseEditor3DWindow::reset() {
    removeClicks();
}
