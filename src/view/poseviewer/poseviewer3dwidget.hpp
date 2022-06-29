#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
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
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlitFramebuffer>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QStencilOperation>

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

    // Scene
    // Scene root will be added as a child
    Qt3DCore::QEntity *m_root;

    // Offscreen framegraph
    QOffscreenSurface *m_offscreenSurface;
    Qt3DRender::QRenderStateSet *m_renderStateSet;
    Qt3DRender::QDepthTest *m_posesDepthTest;
    Qt3DRender::QMultiSampleAntiAliasing *m_multisampleAntialiasing;
    Qt3DRender::QRenderTargetSelector *m_renderTargetSelector;
    Qt3DRender::QRenderSurfaceSelector *m_renderSurfaceSelector;
    Qt3DRender::QRenderTarget *m_renderTarget;
    Qt3DRender::QRenderTargetOutput *m_colorOutput;
    Qt3DRender::QTexture2DMultisample *m_colorTexture;
    Qt3DRender::QRenderTargetOutput *m_depthOutput;
    Qt3DRender::QTexture2DMultisample *m_depthTexture;

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

    // Root entity
    Qt3DCore::QEntity *m_sceneRoot;

    // Base framegraph
    Qt3DRender::QViewport *m_viewport;

    // First branch - clear the buffers
    Qt3DRender::QClearBuffers *m_clearBuffers;
    Qt3DRender::QNoDraw *m_noDraw;

    // Draw poses into texture with depth and stencil
    Qt3DRender::QLayerFilter *m_posesTexturePosesLayerFilter;
    Qt3DRender::QLayer *m_posesTexturePosesLayer;
    Qt3DRender::QRenderTargetSelector *m_posesTextureRenderTargetSelector;
    // Poses render states
    Qt3DRender::QRenderStateSet *m_posesTextureRenderStateSet;
    Qt3DRender::QDepthTest *m_posesTexturePosesDepthTest;
    Qt3DRender::QBlendEquationArguments *m_posesTexturePosesBlendState;
    Qt3DRender::QBlendEquation *m_posesTexturePosesBlendEquation;
    Qt3DRender::QFrustumCulling *m_posesTexturePosesFrustumCulling;
    // Poses camera
    Qt3DRender::QCamera *m_posesTexturePosesCamera;
    Qt3DRender::QCameraSelector *m_posesTexturePosesCameraSelector;
    // Poses texture targets
    Qt3DRender::QMultiSampleAntiAliasing *m_posesTextureMultisampleAntialiasing;
    Qt3DRender::QRenderTarget *m_posesTextureRenderTarget;
    Qt3DRender::QRenderTargetOutput *m_posesTextureColorOutput;
    Qt3DRender::QTexture2DMultisample *m_posesTextureColorTexture;
    Qt3DRender::QRenderTargetOutput *m_posesTextureDepthOutput;
    Qt3DRender::QTexture2DMultisample *m_posesTextureDepthTexture;

    // Don't clear depth here! We need it for drawing the highlighted pose.

    // Draw the highlighted pose to the texture and also to the stencil buffer
    Qt3DRender::QLayerFilter *m_highlightedPoseLayerFilter;
    Qt3DRender::QLayer *m_highlightedPosePosesLayer;
    // We'll reuse the render states from the poses texture branch but
    // also add the stencil operation
    Qt3DRender::QRenderStateSet *m_highlightedPoseRenderStateSet;
    Qt3DRender::QStencilOperation *m_highlightedPoseStencilOperation;
    // We'll reuse the camera from the poses texture branch
    Qt3DRender::QCameraSelector *m_highlightedPoseCameraSelector;
    // We'll reuse the targets from the poses texture branch but this time
    // add a stencil texture
    Qt3DRender::QRenderTargetSelector *m_highlightedPoseRenderTargetSelector;
    Qt3DRender::QRenderTarget *m_highlightedPoseRenderTarget;
    Qt3DRender::QRenderTargetOutput *m_highlightedPosStencilOutput;
    Qt3DRender::QTexture2DMultisample *m_highlightedPosStencilTexture;

    // Multisample blitting of poses rendered to texture
    Qt3DRender::QNoDraw *m_posesTextureBlitNoDraw;
    Qt3DRender::QBlitFramebuffer *m_posesTextureBlitFrameBuffer;
    Qt3DRender::QRenderTarget *m_posesTextureBlitRenderTarget;
    // We don't need to blit depth, otherwise we would have to clear
    // it before drawing the background image to be safe
    Qt3DRender::QRenderTargetOutput *m_posesTextureBlitColorOutput;
    Qt3DRender::QTexture2D *m_posesTextureBlitColorTexture;
    Qt3DRender::QRenderTargetOutput *m_posesTextureBlitStencilOutput;
    Qt3DRender::QTexture2D *m_posesTextureBlitStencilTexture;

    // Background branch
    Qt3DRender::QLayerFilter *m_backgroundLayerFilter;
    Qt3DRender::QLayer *m_backgroundLayer;
    Qt3DRender::QCamera *m_backgroundCamera;
    Qt3DRender::QCameraSelector *m_backgroundCameraSelector;
    Qt3DRender::QNoDepthMask *m_backgroundNoDepthMask;
    Qt3DRender::QNoPicking *m_backgroundNoPicking;
    // QPointer because then it's NULL in the beginning
    QPointer<BackgroundImageRenderable> m_backgroundImageRenderable;

    // Clear depth for poses texture
    Qt3DRender::QClearBuffers *m_clearBuffersAfterBackground;
    Qt3DRender::QNoDraw *m_noDrawAfterBackground;

    // Poses texture branch, which draws the poses rendered to a texture
    // as a quad over the background
    Qt3DRender::QLayerFilter *m_poseTextureLayerFilter;
    Qt3DRender::QLayer *m_poseTextureLayer;
    // We'll reuse the background camera
    Qt3DRender::QCameraSelector *m_poseTextureCameraSelector;
    // Stuff to perform the snapshot
    Qt3DRender::QRenderCapture *m_snapshotRenderCapture;
    Qt3DRender::QRenderCaptureReply *m_snapshotRenderCaptureReply;
    Qt3DExtras::QTextureMaterial *m_posesTextureTextureMaterial;
    Qt3DExtras::QPlaneMesh *m_posesTextureQuad;

    // Clear depth for highlights
    Qt3DRender::QClearBuffers *m_clearBuffersAfterPosesTexture;
    Qt3DRender::QNoDraw *m_noDrawAfterPosesTexture;

    // Draw highlights around the selected pose
    Qt3DRender::QLayerFilter *m_posesHighlightLayerFilter;
    Qt3DRender::QLayer *m_posesHighlightLayer;
    // We'll reuse the background camera
    Qt3DRender::QCameraSelector *m_posesHighlightCameraSelector;
    Qt3DExtras::QTextureMaterial *m_posesHighlightTextureMaterial;
    Qt3DExtras::QPlaneMesh *m_posesHighlightQuad;

    // Clear depth for gizmo
    Qt3DRender::QClearBuffers *m_clearBuffersAfterPosesHighlight;
    Qt3DRender::QNoDraw *m_noDrawAfterPosesHighlight;

    // Gizmo branch
    Qt3DRender::QLayerFilter *m_gizmoLayerFilter;
    Qt3DRender::QLayer *m_gizmoLayer;
    Qt3DRender::QRenderStateSet *m_gizmoRenderStateSet;
    Qt3DRender::QDepthTest *m_gizmoDepthTest;
    Qt3DRender::QCameraSelector *m_gizmoCameraSelector;

    Qt3DRender::QClearBuffers *m_clearBuffersAfterGizmo;
    Qt3DRender::QNoDraw *m_noDrawAfterGizmo;

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
