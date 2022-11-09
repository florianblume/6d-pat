#ifndef OFFSCREENENGINE_H
#define OFFSCREENENGINE_H

#include "view/rendering/texturerendertarget.hpp"
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
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QFilterKey>

// The OffscreenEngine brings together various Qt3D classes that are required in order to
// perform basic scene rendering. Of these, the most important for this project is the OffscreenSurfaceFrameGraph.
// Render captures can be requested, and the capture contents used within other widgets (see OffscreenEngineDelegate).
class OffscreenEngine : public QObject {

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
    Qt3DCore::QAspectEngine *m_aspectEngine;              // The aspect engine, which holds the scene and related aspects.
    Qt3DRender::QRenderAspect *m_renderAspect;            // The render aspect, which deals with rendering the scene.
    Qt3DLogic::QLogicAspect *m_logicAspect;               // The logic aspect, which runs jobs to do with synchronising frames.
    Qt3DRender::QRenderSettings *m_renderSettings;        // The render settings, which control the general rendering behaviour.
    Qt3DRender::QRenderCapture *m_renderCapture;          // The render capture node, which is appended to the frame graph.
    Qt3DCore::QEntity *m_sceneRoot;                         // The scene root, which becomes a child of the engine's root entity.

    Qt3DCore::QEntityPtr m_root;

    OffscreenTextureRenderTarget *m_textureTarget;
    QOffscreenSurface *m_offscreenSurface;
    Qt3DRender::QTechniqueFilter *m_techniqueFilter;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QRenderSurfaceSelector *m_renderSurfaceSelector;
    Qt3DRender::QRenderTargetSelector *m_renderTargetSelector;
    Qt3DRender::QViewport *m_viewport;
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QNoDraw *m_noDraw;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QCamera *m_camera;

    Qt3DRender::QRenderCaptureReply *m_reply;

    ObjectModelRenderable *m_objectModelRenderable;
    Qt3DCore::QEntity *m_lightEntity;
    Qt3DRender::QPointLight *m_light;

    bool m_initialized = false;
};

#endif // OFFSCREENENGINE_H
