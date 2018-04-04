#ifndef OFFSCREENLAYER_H
#define OFFSCREENLAYER_H

#include "texturerendertarget.h"
#include "textureimage.h"
#include <Qt3DRender/QRenderSurfaceSelector>
#include <QOffscreenSurface>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QNoDraw>

class OffscreenSurfaceFrameGraph : public Qt3DRender::QRenderSurfaceSelector
{
public:
    OffscreenSurfaceFrameGraph(Qt3DCore::QNode* parent = nullptr, Qt3DRender::QCamera *camera = nullptr, const QSize &size = QSize(500, 500));
    void setSize(const QSize &size);
    Qt3DCore::QNode *getLastNode();
    void setObjectsCamera(Qt3DRender::QCamera *camera);
    Qt3DCore::QEntity *objectsCamera();
    void setBackgroundImagePath(const QString &path);
    void addLayerToObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer);
    void removeLayerFromObjectsLayerFilter(Qt3DRender::QLayer *objectsLayer);
    void setRootEntity(Qt3DCore::QEntity *rootEntity);

private:
    TextureRenderTarget *textureTarget;
    QOffscreenSurface *offscreenSurface;
    Qt3DRender::QRenderTargetSelector *renderTargetSelector;
    Qt3DRender::QClearBuffers *mainClearBuffers;

    // Background framegraph objects
    Qt3DRender::QLayerFilter *backgroundLayerFilter;
    Qt3DRender::QLayer *backgroundLayer;
    Qt3DRender::QCameraSelector *backgroundCameraSelector;
    Qt3DRender::QCamera *backgroundCamera;

    // The actual background image and its source path
    Qt3DCore::QEntity *backgroundImageEntity;
    Qt3DExtras::QPlaneMesh *backgroundImageMesh;
    Qt3DExtras::QTextureMaterial *backgroundImageMaterial;
    Qt3DRender::QTexture2D *backgroundImageTexture;
    TextureImage *backgroundImageTextureImage;
    Qt3DCore::QTransform *backgroundImageTransform;
    QString backgroundImagePath;

    // Object models framegraph objects
    Qt3DRender::QLayerFilter *objectsLayerFilter;
    Qt3DRender::QCameraSelector *objectsCameraSelector;
    Qt3DRender::QClearBuffers *objectsClearBuffers;
    Qt3DRender::QRenderStateSet *renderStateSet;
    Qt3DRender::QDepthTest *depthTest;

    // Last node for render capture reply
    Qt3DRender::QNoDraw *lastNode;
};

#endif // OFFSCREENLAYER_H
