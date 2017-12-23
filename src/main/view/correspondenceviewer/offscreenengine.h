#ifndef OFFSCREENENGINE_H
#define OFFSCREENENGINE_H

#include "offscreensurfaceframegraph.h"

#include <QSharedPointer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DLogic/QLogicAspect>

class OffscreenEngine
{
public:
    OffscreenEngine(Qt3DRender::QCamera *camera, const QSize &size);
    void setSceneRoot(Qt3DCore::QNode *sceneRoot);
    Qt3DRender::QRenderCapture *getRenderCapture();
    void setSize(const QSize &size);

private:
    Qt3DCore::QAspectEngine *aspectEngine;
    Qt3DRender::QRenderAspect *renderAspect;
    Qt3DLogic::QLogicAspect *logicAspect;
    QSharedPointer<Qt3DCore::QEntity> root;
    Qt3DRender::QRenderSettings *renderSettings;
    Qt3DRender::QRenderCapture *renderCapture;
    OffscreenSurfaceFrameGraph *offscreenFrameGraph;
    Qt3DCore::QNode *sceneRoot;
};

#endif // OFFSCREENENGINE_H
