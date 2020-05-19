#include "view/poseeditor/poseeditor3dwindow.hpp"
#include "misc/global.hpp"

#include <Qt3DRender/QCamera>

PoseEditor3DWindow::PoseEditor3DWindow()
    : Qt3DExtras::Qt3DWindow(),
      rootEntity(new Qt3DCore::QEntity),
      sceneLoader(new Qt3DRender::QSceneLoader(rootEntity)){
    QSurfaceFormat format;
    format.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    format.setStencilBufferSize(STENCIL_BUFFER_SIZE);
    format.setSamples(NUMBER_OF_SAMPLES);
    setFormat(format);

    setRootEntity(rootEntity);
    sceneLoader->setEnabled(false);
    rootEntity->addComponent(sceneLoader);
    connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged,
            this, &PoseEditor3DWindow::onSceneLoaderStatusChanged);
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel *objectModel) {
    sceneLoader->setSource(QUrl::fromLocalFile(objectModel->getAbsolutePath()));
}

void PoseEditor3DWindow::setRotationOfObjectModel(QVector3D rotation) {
    if (!objectModel.isNull()) {
    }
}

PoseEditor3DWindow::~PoseEditor3DWindow() {
}

void PoseEditor3DWindow::onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status) {
    if (status == Qt3DRender::QSceneLoader::Ready) {
        // React to loaded scene
        sceneLoader->setEnabled(true);
        camera()->viewEntity(rootEntity);
    }
}

void PoseEditor3DWindow::addClick(QVector3D position, QColor color) {
    clicks3D.append(position);
    clickColors.append(QVector3D(color.red() / 255.f,
                               color.green() / 255.f,
                               color.blue() / 255.f));
}

void PoseEditor3DWindow::removeClicks() {
    clicks3D.clear();
}

void PoseEditor3DWindow::reset() {
    removeClicks();
    sceneLoader->setEnabled(false);
}
