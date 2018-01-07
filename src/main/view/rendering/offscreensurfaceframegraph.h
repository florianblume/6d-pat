#ifndef OFFSCREENLAYER_H
#define OFFSCREENLAYER_H

#include "texturerendertarget.h"
#include <Qt3DRender/QRenderSurfaceSelector>
#include <QOffscreenSurface>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QMultiSampleAntiAliasing>

class OffscreenSurfaceFrameGraph : public Qt3DRender::QRenderSurfaceSelector
{
public:
    OffscreenSurfaceFrameGraph(Qt3DCore::QNode* parent = nullptr, Qt3DRender::QCamera *camera = nullptr, const QSize &size = QSize(500, 500));
    void setSize(const QSize &size);
    Qt3DCore::QNode *getLastNode();

private:
    TextureRenderTarget *textureTarget;
    QOffscreenSurface *offscreenSurface;
    Qt3DRender::QRenderTargetSelector *renderTargetSelector;
    Qt3DRender::QViewport *viewport;
    Qt3DRender::QClearBuffers *clearBuffers;
    Qt3DRender::QCameraSelector *cameraSelector;
    Qt3DRender::QCamera *camera;
    Qt3DRender::QRenderStateSet *renderStateSet;
    Qt3DRender::QMultiSampleAntiAliasing *multiSample;

};

#endif // OFFSCREENLAYER_H
