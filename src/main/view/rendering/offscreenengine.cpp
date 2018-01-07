#include "offscreenengine.h"

OffscreenEngine::OffscreenEngine(Qt3DRender::QCamera *camera, const QSize &size) {
    aspectEngine = new Qt3DCore::QAspectEngine();
    renderAspect = new Qt3DRender::QRenderAspect();
    logicAspect = new Qt3DLogic::QLogicAspect();
    aspectEngine->registerAspect(renderAspect);
    aspectEngine->registerAspect(logicAspect);
    root.reset(new Qt3DCore::QEntity());
    renderSettings = new Qt3DRender::QRenderSettings(root.data());
    root->addComponent(renderSettings);
    offscreenFrameGraph = new OffscreenSurfaceFrameGraph(renderSettings, camera, size);
    renderSettings->setActiveFrameGraph(offscreenFrameGraph);
    renderCapture = new Qt3DRender::QRenderCapture(offscreenFrameGraph->getLastNode());
    aspectEngine->setRootEntity(root);
}

void OffscreenEngine::setSceneRoot(Qt3DCore::QNode *sceneRoot) {
    this->sceneRoot = sceneRoot;
    sceneRoot->setParent(root.data());
}

Qt3DRender::QRenderCapture* OffscreenEngine::getRenderCapture() {
    return renderCapture;
}

void OffscreenEngine::setSize(const QSize &size) {
    offscreenFrameGraph->setSize(size);
}

void OffscreenEngine::start() {
    // This starts the graphics loop
    //aspectEngine->setRootEntity(root);
}

void OffscreenEngine::stop() {
    //aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());
}
