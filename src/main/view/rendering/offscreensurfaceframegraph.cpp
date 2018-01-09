#include "offscreensurfaceframegraph.h"

#include <Qt3DRender/QNoDraw>

OffscreenSurfaceFrameGraph::OffscreenSurfaceFrameGraph(Qt3DCore::QNode* parent, Qt3DRender::QCamera *camera, const QSize &size) :
    Qt3DRender::QRenderSurfaceSelector(parent),
    camera(camera)
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

    clearBuffers = new Qt3DRender::QClearBuffers(renderTargetSelector);
    // Does not matter, as we draw the image over the whole background anyway
    clearBuffers->setClearColor(QColor(255, 255, 255, 0));
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthStencilBuffer);

    // We don't want any drawing at this end
    Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(clearBuffers);

    viewport = new Qt3DRender::QViewport(renderTargetSelector);
    viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));

    cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(camera);

    renderStateSet = new Qt3DRender::QRenderStateSet(cameraSelector);
    depthTest = new Qt3DRender::QDepthTest(renderStateSet);
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
    renderStateSet->addRenderState(depthTest);
}

void OffscreenSurfaceFrameGraph::setSize(const QSize &size) {
    textureTarget->setSize(size);
    setExternalRenderTargetSize(size);
}

Qt3DCore::QNode *OffscreenSurfaceFrameGraph::getLastNode() {
    return cameraSelector;
}
