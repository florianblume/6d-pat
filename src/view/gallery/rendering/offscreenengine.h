#ifndef OFFSCREENENGINE_H
#define OFFSCREENENGINE_H

#include "texturerendertarget.h"
#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"

#include <QObject>

#include <QSharedPointer>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DLogic/QLogicAspect>

#include <QOffscreenSurface>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QSceneLoader>

// The OffscreenEngine brings together various Qt3D classes that are required in order to
// perform basic scene rendering. Of these, the most important for this project is the OffscreenSurfaceFrameGraph.
// Render captures can be requested, and the capture contents used within other widgets (see OffscreenEngineDelegate).
class OffscreenEngine : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)

public:
    OffscreenEngine(const QSize &size);
    ~OffscreenEngine();

    void setObjectModel(const ObjectModel &objectModel);
    void setBackgroundColor(QColor color);
    void setSize(const QSize &size);
    QSize size();

public Q_SLOTS:
    void requestImage();

Q_SIGNALS:
    void imageReady(QImage image);
    void sizeChanged(QSize newSize);

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void onRenderCaptureReady();
    void shutdown();

private:
    // We need all of the following in order to render a scene:
    Qt3DCore::QAspectEngine *aspectEngine;              // The aspect engine, which holds the scene and related aspects.
    Qt3DRender::QRenderAspect *renderAspect;            // The render aspect, which deals with rendering the scene.
    Qt3DLogic::QLogicAspect *logicAspect;               // The logic aspect, which runs jobs to do with synchronising frames.
    Qt3DRender::QRenderSettings *renderSettings;        // The render settings, which control the general rendering behaviour.
    Qt3DRender::QRenderCapture *renderCapture;          // The render capture node, which is appended to the frame graph.
    Qt3DCore::QEntity *sceneRoot;                         // The scene root, which becomes a child of the engine's root entity.

    TextureRenderTarget *textureTarget;
    QOffscreenSurface *offscreenSurface;
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector;
    Qt3DRender::QRenderTargetSelector *renderTargetSelector;
    Qt3DRender::QViewport *viewport;
    Qt3DRender::QClearBuffers *clearBuffers;
    Qt3DRender::QCameraSelector *cameraSelector;
    Qt3DRender::QCamera *camera;

    Qt3DRender::QRenderCaptureReply *reply;

    ObjectModelRenderable *objectModelRenderable;
    Qt3DRender::QPointLight *light;

    bool initialized = false;
};

#endif // OFFSCREENENGINE_H
