#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
#include "view/rendering/outlinerenderable.hpp"
#include "view/rendering/arcballrotationhandler.hpp"
#include "view/rendering/translationhandler.hpp"
#include "view/rendering/gizmo/qt3dgizmo.hpp"
#include "view/poseviewer/mousecoordinatesmodificationeventfilter.hpp"
#include "view/poseviewer/undomousecoordinatesmodificationeventfilter.hpp"
#include "settings/settings.hpp"

#include <QString>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QList>
#include <QMatrix4x4>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QObject>
#include <QOffscreenSurface>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QElapsedTimer>
#include <QTimer>

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QNoPicking>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>
#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilOperation>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilOperationArguments>
#include <Qt3DRender/QStencilTestArguments>

class PoseViewer3DWidget : public QOpenGLWidget
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE setObjectsOpacity)
    Q_PROPERTY(float zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QPoint renderingPosition READ renderingPosition WRITE setRenderingPosition)

public:
    explicit PoseViewer3DWidget(QWidget *parent = nullptr);
    ~PoseViewer3DWidget();

    void reset();

    void initializeGL() override;
    //void resizeGL(int w, int h) override;

    void setSettings(SettingsPtr settings);

    // Data related methods
    void setBackgroundImage(const QString& image, const QMatrix3x3 &cameraMatrix,
                            float nearPlane, float farPlane);
    void setPoses(const QList<PosePtr> &poses);
    void addPose(PosePtr pose);
    void removePose(PosePtr pose);
    void selectPose(PosePtr selected, PosePtr deselected);
    void setObjectsOpacity(float opacity);
    void setAnimatedObjectsOpacity(float opacity);
    float opacity();
    void setClicks(const QList<QPoint> &clicks);
    QSize imageSize() const;

    // Rendering related methods
    void setSamples(int samples);
    void setRenderingPosition(float x, float y);
    void setRenderingPosition(QPoint position);
    void setAnimatedRenderingPosition(float x, float y);
    void setAnimatedRenderingPosition(QPoint position);
    QPoint renderingPosition();
    int zoom();
    void setZoom(int zoom);
    void setAnimatedZoom(int zoom);
    void setAnimatedZoomAndRenderingPosition(int zoom, float x, float y);
    void setAnimatedZoomAndRenderingPosition(int zoom, QPoint renderingPosition);

    // Mouse events
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void leaveEvent(QEvent *event) override;

public Q_SLOTS:
    // Main paint function of QOpenGLWidget called whenever
    // Qt has a frame ready
    void paintGL() override;
    void takeSnapshot(const QString &path);

Q_SIGNALS:
    void positionClicked(QPoint position);
    void poseSelected(PosePtr pose);
    void snapshotSaved();
    void zoomChanged(int zoom);
    void opacityChanged(float opacity);

private Q_SLOTS:
    // Called by Qt3D when the snapshot is ready
    void onSnapshotReady();
    void onGizmoIsTranslating();
    void onGizmoIsRotating();
    void onGizmoTransformingEnded();

private:
    void init();
    void initOpenGL();
    void initQt3D();
    void setRenderingSize(const QSize &size);
    void setupZoomAnimation(int zoom);
    void setupRenderingPositionAnimation(int x, int y);
    void setupRenderingPositionAnimation(QPoint reinderingPosition);

private:
    PosePtr m_selectedPose;
    PoseRenderable *m_selectedPoseRenderable = Q_NULLPTR;
    PoseRenderable *m_hoveredPose = Q_NULLPTR;
    // The size of the loaded image
    QSize m_imageSize;
    SettingsPtr m_settings;
    QLabel *m_fpsLabel;
    QElapsedTimer m_elapsedTimer;
    qint64 m_elapsed = 1;
    float m_avgElapsed = 1.0;
    float m_fpsAlpha = 0.9;
    QTimer m_updateFPSLabelTimer;

    /*!
     *
     * Offscreen rendering framegraph. The widget renders everything using
     * Qt3D into an offscreen texture which is then used to draw the texture
     * of a quad in the paintGL() function.
     *
     */

    int m_samples = 1;

    Qt3DCore::QAspectEngine *m_aspectEngine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    // Renderer configuration
    Qt3DRender::QRenderSettings *m_renderSettings;

    // Input configuration
    Qt3DInput::QInputSettings *m_inputSettings;

    // To get notified when a frame is ready
    Qt3DLogic::QFrameAction *m_frameAction;

    // OpenGL setup
    bool m_initialized;

    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QVector<GLfloat> m_vertexData;
    QOpenGLTexture *m_texture = Q_NULLPTR;
    const int m_vertexAttributeLoc = 0;
    const int m_texCoordAttributeLoc = 1;

    // Parameters of the whole rendering
    QPoint m_renderingPosition = QPoint(0, 0);
    // Default scale is 1
    float m_renderingScale = 1.f;
    // Default zoom is 100%
    int m_zoom = 100;

    /*!
     *
     * The actual frame graph for rendering the background image,
     * the poses and the clicks.
     *
     *                                                      root
     *                                                      |
     *     -------------------------------------------------------------------------------------------
     *     |                  |               |             |          |                  |          |
     *  Clear buffers   Draw background   Draw poses   Clear depth   Draw gizmo   Clear depth   Draw clicks
     *                      image
     *
     */

    // Scene
    // Scene root will be added as a child
    Qt3DCore::QEntity *m_root;

    // Root entity
    Qt3DCore::QEntity *m_sceneRoot;

    // Base framegraph drawing offscreen
    QOffscreenSurface *m_offscreenSurface;
    Qt3DRender::QRenderSurfaceSelector *m_renderSurfaceSelector;
    Qt3DRender::QViewport *m_viewport;

    // Render stuff for outline framegraph
    Qt3DRender::QTechniqueFilter *m_posesOutlineTechniqueFilter;
    Qt3DRender::QFilterKey *m_posesOutlineFilterKey;
    Qt3DRender::QCameraSelector *m_posesOutlineCameraSelector;
    // For the highlighted
    Qt3DRender::QLayerFilter *m_posesOutlineHighlightedLayerFilter;
    Qt3DRender::QLayer *m_posesOutlineHighlightedLayer;
    Qt3DRender::QRenderTargetSelector *m_posesOutlineHighlightedRenderTargetSelector;
    Qt3DRender::QRenderTarget *m_posesOutlineHighlightedRenderTarget;
    Qt3DRender::QRenderTargetOutput *m_posesOutlineHighlightedOutput;
    Qt3DRender::QTexture2D *m_posesOutlineHighlightedTexture;
    Qt3DRender::QClearBuffers *m_posesOutlineHighlightedClearBuffers;
    Qt3DRender::QNoDraw *m_posesOutlineHighlightedNoDraw;
    // For the selected
    Qt3DRender::QLayerFilter *m_posesOutlineSelectedLayerFilter;
    Qt3DRender::QLayer *m_posesOutlineSelectedLayer;
    Qt3DRender::QRenderTargetSelector *m_posesOutlineSelectedRenderTargetSelector;
    Qt3DRender::QRenderTarget *m_posesOutlineSelectedRenderTarget;
    Qt3DRender::QRenderTargetOutput *m_posesOutlineSelectedOutput;
    Qt3DRender::QTexture2D *m_posesOutlineSelectedTexture;
    Qt3DRender::QClearBuffers *m_posesOutlineSelectedClearBuffers;
    Qt3DRender::QNoDraw *m_posesOutlineSelectedNoDraw;

    // Normal framegraph
    Qt3DRender::QTechniqueFilter *m_mainTechniqueFilter;
    Qt3DRender::QFilterKey *m_mainFilterKey;
    Qt3DRender::QRenderTargetSelector *m_renderTargetSelector;
    Qt3DRender::QRenderTarget *m_renderTargetMS;
    Qt3DRender::QRenderTargetOutput *m_colorOutputMS;
    Qt3DRender::QTexture2DMultisample *m_colorTextureMS;
    Qt3DRender::QRenderTargetOutput *m_depthStencilOutputMS;
    Qt3DRender::QTexture2DMultisample *m_depthStencilTextureMS;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QDepthTest *m_posesDepthTest;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;

    // First branch - clear the buffers
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QNoDraw *m_noDraw;

    // Background branch
    Qt3DRender::QLayerFilter *m_backgroundLayerFilter;
    Qt3DRender::QLayer *m_backgroundLayer;
    Qt3DRender::QCamera *m_backgroundCamera;
    Qt3DRender::QCameraSelector *m_backgroundCameraSelector;
    Qt3DRender::QNoDepthMask *m_backgroundNoDepthMask;
    Qt3DRender::QNoPicking *m_backgroundNoPicking;
    QPointer<BackgroundImageRenderable> m_backgroundImageRenderable;

    // Poses branch
    Qt3DRender::QLayerFilter *m_posesLayerFilter;
    Qt3DRender::QLayer *m_posesLayer;
    Qt3DRender::QRenderStateSet *m_posesRenderStateSet;
    Qt3DRender::QBlendEquationArguments *m_posesBlendState;
    Qt3DRender::QBlendEquation *m_posesBlendEquation;
    Qt3DRender::QFrustumCulling *m_posesFrustumCulling;
    // Filter which adds the parameter which removes the highlight color
    // Must be before the rest which draws the objects
    Qt3DRender::QRenderPassFilter *m_snapshotRenderPassFilter;
    Qt3DRender::QParameter *m_removeHighlightParameter;
    // The main part of the poses branch
    Qt3DRender::QCamera *m_posesCamera;
    Qt3DRender::QCameraSelector *m_posesCameraSelector;
    // Stuff to perform the snapshot
    Qt3DRender::QRenderCapture *m_snapshotRenderCapture;
    Qt3DRender::QRenderCaptureReply *m_snapshotRenderCaptureReply;

    // Clear depth buffer before drawing outline, gizmo and clicks
    Qt3DRender::QClearBuffers *m_clearBuffers2;
    Qt3DRender::QNoDraw *m_noDraw2;

    // Outline branch
    Qt3DRender::QLayerFilter *m_outlinePlaneLayerFilter;
    Qt3DRender::QLayer *m_outlinePlaneLayer;
    Qt3DRender::QRenderStateSet *m_outlineRenderStateSet;
    Qt3DRender::QStencilTest *m_outlineStencilTest;
    Qt3DRender::QStencilMask *m_outlineStencilMask;
    // We'll reuse the background camera
    //Qt3DRender::QCamera *m_outlineCamera;
    Qt3DRender::QCameraSelector *m_outlineCameraSelector;
    Qt3DRender::QNoDepthMask *m_outlineNoDepthMask;
    Qt3DRender::QNoPicking *m_outlineNoPicking;
    QPointer<OutlineRenderable> m_outlineHighlightedRenderable;
    QPointer<OutlineRenderable> m_outlineSelectedRenderable;

    // ClickVisualization branch
    Qt3DRender::QLayerFilter *m_gizmoLayerFilter;
    Qt3DRender::QLayer *m_gizmoLayer;
    Qt3DRender::QRenderStateSet *m_gizmoRenderStateSet;
    Qt3DRender::QDepthTest *m_gizmoDepthTest;
    Qt3DRender::QCameraSelector *m_gizmoCameraSelector;

    // ClickVisualization branch
    Qt3DRender::QLayerFilter *m_clickVisualizationLayerFilter;
    Qt3DRender::QLayer *m_clickVisualizationLayer;
    Qt3DRender::QCameraSelector *m_clickVisualizationCameraSelector;
    Qt3DRender::QCamera *m_clickVisualizationCamera;
    // Probably don't need this but can't hurt
    Qt3DRender::QNoDepthMask *m_clickVisualizationNoDepthMask;
    ClickVisualizationRenderable *m_clickVisualizationRenderable;

    QList<PoseRenderable *> m_poseRenderables;
    QMap<QString, PoseRenderable*> m_poseRenderableForId;
    QMatrix4x4 m_projectionMatrix;
    float m_opacity = 1.0;
    // To animate opacity changes
    QScopedPointer<QPropertyAnimation> m_opacityAnimation;

    QMatrix4x4 m_rotationMat;

    Qt3DGizmo *m_gizmo = Q_NULLPTR;
    bool m_gizmoIsTransforming = false;

    float m_depth = 0.f;

    // To handle dragging of the widget and clicking
    QPointF m_firstClickPos;
    QPointF m_newPos;
    QPointF m_currentClickPos;
    // Save the initial rendering position for the mouse move events
    // or
    QPointF m_initialRenderingPosition;
    bool m_mouseMoved = false;
    bool m_mouseOverPoseRenderable = false;
    bool m_poseRenderablePressed = false;
    bool m_poseRenderableTranslated = false;
    bool m_poseRenderableRotated = false;

    // Store the mouse button that was clicked because the Qt3D
    // pick events don't deliver them in their events when the
    // mouse is moved after clicking and holding down the mouse button
    Qt::MouseButton m_clickedMouseButton;

    // Since the orthographic projection in Qt3D uses the width and height of the
    // containing widget (i.e. size of what's actually visible) but we need the
    // coordinates on the (potentially) offset image, we need to modify the mouse
    // coordinates to match the local coordinates of the image before passing
    // them on to Qt3D.
    QScopedPointer<MouseCoordinatesModificationEventFilter> m_mouseCoordinatesModificationEventFilter;
    // Afterwards we undo the modifications so that our widget receives the normal coordinates.
    // Note that we have to add the undo filter first to get it executed last
    QScopedPointer<UndoMouseCoordinatesModificationEventFilter> m_undoMouseCoordinatesModificationEventFilter;
    bool m_mouseCoordinatesModificationEventFilterInstalled = false;

    // Zoom stuff
    QSharedPointer<QPropertyAnimation> m_zoomAnimation;
    int m_zoomNormalizingFactor = 10;
    int m_maxZoom = 200;
    int m_minZoom = 1;
    QSharedPointer<QPropertyAnimation> m_renderingPositionAnimation;

    QSharedPointer<QParallelAnimationGroup> m_zoomAndRenderingPositionAnimationGroup;

    // Stores the path to store the snapshot at between requesting
    // the snapshot and receiving it (Qt3D takes some time until
    // the snapshot is ready and doesn't deliver it immediatly)
    QString m_snapshotPath;

    static const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons> MOUSE_BUTTON_MAPPING;
};

#endif
