#include "offscreensurfaceframegraph.h"

OffscreenSurfaceFrameGraph::OffscreenSurfaceFrameGraph(Qt3DCore::QNode* parent, Qt3DRender::QCamera *camera, const QSize &size) :
    Qt3DRender::QRenderSurfaceSelector(parent),
    camera(camera)
{
    offscreenSurface = new QOffscreenSurface();
    QSurfaceFormat format;
    format.setDepthBufferSize(32);
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

    // Set the viewport for the render target selector so that it knows
    // what to draw
    viewport = new Qt3DRender::QViewport(this);
    viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));
    cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(camera);
    clearBuffers = new Qt3DRender::QClearBuffers(cameraSelector);
    // Does not matter, as we draw the image over the whole background anyway
    clearBuffers->setClearColor(QColor(255, 255, 255, 255));
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
}

void OffscreenSurfaceFrameGraph::setSize(const QSize &size) {
    textureTarget->setSize(size);
    setExternalRenderTargetSize(size);
}
