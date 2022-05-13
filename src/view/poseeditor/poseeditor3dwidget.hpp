#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"
#include "view/rendering/translationhandler.hpp"
#include "view/rendering/arcballrotationhandler.hpp"
#include "view/rendering/gizmo/qt3dgizmo.hpp"
#include "settings/settingsstore.hpp"

#include <QString>
#include <QList>
#include <QVector3D>
#include <QWheelEvent>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCullFace>

class PoseEditor3DWindow : public Qt3DExtras::Qt3DWindow {
    Q_OBJECT

public:
    explicit PoseEditor3DWindow();
    void setObjectModel(const ObjectModel &objectModel);
    void setClicks(const QList<QVector3D> &clicks);
    void reset();
    void setSettingsStore(SettingsStore *settingsStore);
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    ~PoseEditor3DWindow();

public Q_SLOTS:
    void reset3DScene();

Q_SIGNALS:
    void positionClicked(const QVector3D &position);
    void mouseMoved(const QVector3D &position);
    void mouseExited();

private Q_SLOTS:
    void onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void onObjectModelRenderablePressed(Qt3DRender::QPickEvent *event);
    void onObjectModelRenderableMoved(Qt3DRender::QPickEvent *event);
    void onCurrentSettingsChanged(SettingsPtr settings);

private:
    // Framegraph stuff
    // Base framegraph
    Qt3DRender::QRenderSurfaceSelector *m_renderSurfaceSelector;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;
    Qt3DRender::QDepthTest *m_depthTest;
    Qt3DRender::QViewport *m_viewport;

    // First branch - clear the buffers
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QNoDraw *m_noDraw;

    // Draw object
    Qt3DRender::QCamera *m_camera;
    Qt3DRender::QCameraSelector *m_cameraSelector;
    Qt3DRender::QLayerFilter *m_objectModelLayerFilter;
    Qt3DRender::QLayer *m_objectModelLayer;

    // Clear depth buffer before drawing gizmo and clicks
    Qt3DRender::QClearBuffers *m_clearBuffers2;
    Qt3DRender::QNoDraw *m_noDraw2;

    // Gizmo branch to draw it ontop of the object
    Qt3DRender::QCameraSelector *m_gizmoCameraSelector;
    Qt3DRender::QRenderStateSet *m_gizmoRenderStateSet;
    Qt3DRender::QCullFace *m_gizmoCullFace;
    Qt3DRender::QLayerFilter *m_gizmoLayerFilter;
    Qt3DRender::QLayer *m_gizmoLayer;

    // Entity tree stuff
    Qt3DCore::QEntity *m_rootEntity;
    // We need a second root because we attach a transform to it that
    // we can rotate and translate. If we attached this transform to the
    // topmost entity, the light gets transformed, too (bad).
    Qt3DCore::QEntity *m_objectModelRoot;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DCore::QTransform *m_objectModelTransform;
    ObjectModelRenderable *objectModelRenderable = Q_NULLPTR;
    Qt3DCore::QEntity *m_lightEntity;
    Qt3DCore::QTransform *m_lightTransform;

    Qt3DRender::QPickEvent::Buttons m_pressedMouseButton = Qt3DRender::QPickEvent::NoButton;
    Qt3DGizmo *m_gizmo;

    SettingsStore *m_settingsStore = Q_NULLPTR;

    bool m_mouseMoved = false;
    bool m_mouseDownOnObjectModelRenderable = false;
    bool m_mouseMovedOnObjectModelRenderable = false;
};

#endif
