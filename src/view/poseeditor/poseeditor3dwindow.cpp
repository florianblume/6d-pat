#include "view/poseeditor/poseeditor3dwindow.hpp"
#include "misc/global.hpp"

#include <QVector3D>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPickEvent>

#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/qpickingsettings.h>

PoseEditor3DWindow::PoseEditor3DWindow()
    : Qt3DExtras::Qt3DWindow(),
      rootEntity(new Qt3DCore::QEntity),
      sceneLoader(new Qt3DRender::QSceneLoader(rootEntity)),
      picker(new Qt3DRender::QObjectPicker(rootEntity)) {
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    setRootEntity(rootEntity);
    rootEntity->addComponent(sceneLoader);

    Qt3DRender::QCamera *cameraEntity = this->camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);

    this->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    connect(picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){Q_EMIT positionClicked(pickEvent->localIntersection());});
    connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, [](){qDebug() << "test";});
}

PoseEditor3DWindow::~PoseEditor3DWindow() {
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    //sceneLoader->setSource(QUrl::fromLocalFile(objectModel.getAbsolutePath()));
    sceneLoader->setSource(QUrl::fromLocalFile("/home/flo/Documents/035_power_drill/textured.obj"));
}

void PoseEditor3DWindow::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    qDebug() << status;
    if (status == Qt3DRender::QSceneLoader::Ready) {
        camera()->viewAll();
    }
}

void PoseEditor3DWindow::setRotationOfObjectModel(QVector3D rotation) {
}

void PoseEditor3DWindow::addClick(QVector3D position, QColor color) {
}

void PoseEditor3DWindow::removeClicks() {
}

void PoseEditor3DWindow::reset() {
}
