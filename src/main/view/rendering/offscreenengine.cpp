#include "offscreenengine.h"

OffscreenEngine::OffscreenEngine(Qt3DRender::QCamera *camera, const QSize &size, const QPointF &objectsOffset) {
    aspectEngine = new Qt3DCore::QAspectEngine();
    renderAspect = new Qt3DRender::QRenderAspect();
    logicAspect = new Qt3DLogic::QLogicAspect();
    aspectEngine->registerAspect(renderAspect);
    aspectEngine->registerAspect(logicAspect);
    root.reset(new Qt3DCore::QEntity());
    renderSettings = new Qt3DRender::QRenderSettings(root.data());
    root->addComponent(renderSettings);
    offscreenFrameGraph = new OffscreenSurfaceFrameGraph(renderSettings, camera, size, objectsOffset);
    renderSettings->setActiveFrameGraph(offscreenFrameGraph);
    renderCapture = new Qt3DRender::QRenderCapture(offscreenFrameGraph->getLastNode());
    aspectEngine->setRootEntity(root);
    offscreenFrameGraph->setRootEntity(root.data());
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

void OffscreenEngine::setObjectsOffset(const QPointF &objectsOffset) {
    offscreenFrameGraph->setObjectsOffset(objectsOffset);
}

void OffscreenEngine::setBackgroundImagePath(const QString &path){
    offscreenFrameGraph->setBackgroundImagePath(path);
}

void OffscreenEngine::addLayerToObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer) {
    offscreenFrameGraph->addLayerToObjectsLayerFilter(objectsLayer);
}

void OffscreenEngine::removeLayerFromObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer) {
    offscreenFrameGraph->removeLayerFromObjectsLayerFilter(objectsLayer);
}
