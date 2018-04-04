#include "offscreensurfaceframegraph.h"

#include <Qt3DRender/QNoDraw>

OffscreenSurfaceFrameGraph::OffscreenSurfaceFrameGraph(Qt3DCore::QNode* parent,
                                                       Qt3DRender::QCamera *camera,
                                                       const QSize &size,
                                                       const QPointF &objectsOffset) :
    Qt3DRender::QRenderSurfaceSelector(parent)
{
    offscreenSurface = new QOffscreenSurface();
    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);
    offscreenSurface->setFormat(format);
    offscreenSurface->create();
    setSurface(offscreenSurface);
    setExternalRenderTargetSize(size);

    // Direct rendering into a texture
    renderTargetSelector = new Qt3DRender::QRenderTargetSelector(this);
    textureTarget = new TextureRenderTarget(renderTargetSelector, size);
    renderTargetSelector->setTarget(textureTarget);

    // First branch to clear the buffers
    mainClearBuffers = new Qt3DRender::QClearBuffers(renderTargetSelector);
    mainClearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    mainClearBuffers->setClearColor(Qt::black);

    // We don't want any drawing at this end
    Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(mainClearBuffers);

    // Second branch for the background image
    backgroundLayerFilter = new Qt3DRender::QLayerFilter(renderTargetSelector);
    backgroundLayer = new Qt3DRender::QLayer(backgroundLayerFilter);
    backgroundLayerFilter->addLayer(backgroundLayer);
    backgroundCameraSelector = new Qt3DRender::QCameraSelector(backgroundLayerFilter);
    backgroundCamera = new Qt3DRender::QCamera(backgroundCameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    backgroundCameraSelector->setCamera(backgroundCamera);

    // The background image
    backgroundImageEntity = new Qt3DCore::QEntity();
    backgroundImageEntity->addComponent(backgroundLayer);
    backgroundImageMesh = new Qt3DExtras::QPlaneMesh(backgroundImageEntity);
    backgroundImageMesh->setHeight(2);
    backgroundImageMesh->setWidth(2);
    backgroundImageEntity->addComponent(backgroundImageMesh);
    backgroundImageMaterial = new Qt3DExtras::QTextureMaterial(backgroundImageEntity);
    backgroundImageTexture = new Qt3DRender::QTexture2D(backgroundImageMaterial);
    backgroundImageTextureImage = new TextureImage(backgroundImageTexture);
    // placeholder size
    backgroundImageTextureImage->setSize(QSize(50, 50));
    backgroundImageTexture->addTextureImage(backgroundImageTextureImage);
    backgroundImageMaterial->setTexture(backgroundImageTexture);
    backgroundImageEntity->addComponent(backgroundImageMaterial);
    backgroundImageTransform = new Qt3DCore::QTransform(backgroundImageEntity);
    backgroundImageTransform->setRotationX(90);
    backgroundImageEntity->addComponent(backgroundImageTransform);

    // Third branch for the actual objects
    objectsLayerFilter = new Qt3DRender::QLayerFilter(renderTargetSelector);
    objectsViewport = new Qt3DRender::QViewport(objectsLayerFilter);
    objectsViewport->setNormalizedRect(QRectF(objectsOffset.x(), objectsOffset.y(), 1.f, 1.f));
    objectsCameraSelector = new Qt3DRender::QCameraSelector(objectsViewport);
    objectsCameraSelector->setCamera(camera);
    objectsClearBuffers = new Qt3DRender::QClearBuffers(objectsCameraSelector);
    // Clear depth buffer so that objects will always be drawn above background
    objectsClearBuffers->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);

    renderStateSet = new Qt3DRender::QRenderStateSet(objectsClearBuffers);
    depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
    renderStateSet->addRenderState(depthTest);

    lastNode = new Qt3DRender::QNoDraw(renderTargetSelector);
}

void OffscreenSurfaceFrameGraph::setSize(const QSize &size) {
    backgroundImageTextureImage->setSize(size);
    textureTarget->setSize(size);
    setExternalRenderTargetSize(size);
}

void OffscreenSurfaceFrameGraph::setObjectsOffset(const QPointF objectsOffset) {
    objectsViewport->setNormalizedRect(QRectF(objectsOffset.x(), objectsOffset.y(), 1.f, 1.f));
}

Qt3DCore::QNode *OffscreenSurfaceFrameGraph::getLastNode() {
    return lastNode;
}

void OffscreenSurfaceFrameGraph::setObjectsCamera(Qt3DRender::QCamera *camera) {
    objectsCameraSelector->setCamera(camera);
}

Qt3DCore::QEntity *OffscreenSurfaceFrameGraph::objectsCamera() {
    return objectsCameraSelector->camera();
}

void OffscreenSurfaceFrameGraph::setBackgroundImagePath(const QString &path) {
    this->backgroundImagePath = path;
    // Automatically updates the image in the code of TextureImage
    backgroundImageTextureImage->setImagePath(path);
}

void OffscreenSurfaceFrameGraph::addLayerToObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer) {
    objectsLayerFilter->addLayer(objectsLayer);
}

void OffscreenSurfaceFrameGraph::removeLayerFromObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer) {
    objectsLayerFilter->removeLayer(objectsLayer);
}

void OffscreenSurfaceFrameGraph::setRootEntity(Qt3DCore::QEntity *rootEntity) {
    backgroundImageEntity->setParent(rootEntity);
}
