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
    renderCapture = new Qt3DRender::QRenderCapture(renderSettings);
    renderSettings->setActiveFrameGraph(renderCapture);
    offscreenFrameGraph = new OffscreenSurfaceFrameGraph(renderCapture, camera, size);
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
