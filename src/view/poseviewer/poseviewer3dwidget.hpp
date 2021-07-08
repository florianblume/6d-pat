#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
#include "view/qt3dwidget/qt3dwidget.hpp"
#include "mousecoordinatesmodificationeventfilter.hpp"
#include "settings/settings.hpp"

#include <QString>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QList>
#include <QMatrix4x4>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QObject>

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
#include <QPropertyAnimation>

class PoseViewer3DWidget : public Qt3DWidget
{
    Q_OBJECT

    Q_PROPERTY(float opacity WRITE setObjectsOpacity)

public:
    explicit PoseViewer3DWidget(QWidget *parent = nullptr);
    ~PoseViewer3DWidget();

    void initializeQt3D() override;

    void setBackgroundImage(const QString& image, const QMatrix3x3 &cameraMatrix,
                            float nearPlane, float farPlane);
    void setPoses(const QList<PosePtr> &poses);
    void addPose(PosePtr pose);
    void removePose(PosePtr pose);

    void selectPose(PosePtr selected, PosePtr deselected);
    void setObjectsOpacity(float opacity);
    void setAnimatedObjectsOpacity(float opacity);
    void setClicks(const QList<QPoint> &clicks);
    QSize imageSize() const;
    void reset();
    void setSettings(SettingsPtr settings);

public Q_SLOTS:
    // React to object pickers
    void onPoseRenderableMoved(Qt3DRender::QPickEvent *pickEvent);
    void onPoseRenderablePressed(Qt3DRender::QPickEvent *pickEvent);
    void takeSnapshot(const QString &path);

Q_SIGNALS:
    void positionClicked(QPoint position);
    void poseSelected(PosePtr pose);
    void snapshotSaved();
    void zoomChanged(float zoom);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void onSnapshotReady();

private:
    QVector3D arcBallVectorForMousePos(const QPointF pos);

private:
    // Root entity
    Qt3DCore::QEntity *root;

    // Base framegraph
    Qt3DRender::QViewport *viewport;

    // First branch - clear the buffers
    Qt3DRender::QClearBuffers *clearBuffers;
    Qt3DRender::QNoDraw *noDraw;

    // Background branch
    Qt3DRender::QLayerFilter *backgroundLayerFilter;
    Qt3DRender::QLayer *backgroundLayer;
    Qt3DRender::QCamera *backgroundCamera;
    Qt3DRender::QCameraSelector *backgroundCameraSelector;
    Qt3DRender::QNoDepthMask *backgroundNoDepthMask;
    Qt3DRender::QNoPicking *backgroundNoPicking;
    QPointer<BackgroundImageRenderable> backgroundImageRenderable;

    // Poses branch
    Qt3DRender::QLayerFilter *posesLayerFilter;
    Qt3DRender::QLayer *posesLayer;
    Qt3DRender::QFrustumCulling *posesFrustumCulling;
    // Filter which adds the parameter which removes the highlight color
    // Must be before the rest which draws the objects
    Qt3DRender::QRenderPassFilter *snapshotRenderPassFilter;
    Qt3DRender::QParameter *removeHighlightParameter;
    // The main part of the poses branch
    Qt3DRender::QCamera *posesCamera;
    Qt3DRender::QCameraSelector *posesCameraSelector;
    // Stuff to perform the snapshot
    Qt3DRender::QRenderCapture *snapshotRenderCapture;
    Qt3DRender::QRenderCaptureReply *snapshotRenderCaptureReply;

    // Clear depth buffer before drawing clicks
    Qt3DRender::QClearBuffers *clearBuffers2;
    Qt3DRender::QNoDraw *noDraw2;

    // ClickVisualization branch
    Qt3DRender::QLayerFilter *clickVisualizationLayerFilter;
    Qt3DRender::QLayer *clickVisualizationLayer;
    Qt3DRender::QCameraSelector *clickVisualizationCameraSelector;
    Qt3DRender::QCamera *clickVisualizationCamera;
    Qt3DRender::QNoDepthMask *clickVisualizationNoDepthMask;
    ClickVisualizationRenderable *clickVisualizationRenderable;

    QList<PoseRenderable *> poseRenderables;
    QMap<QString, PoseRenderable*> poseRenderableForId;
    QMatrix4x4 projectionMatrix;
    float opacity = 1.0;
    QScopedPointer<QPropertyAnimation> opacityAnimation;

    QMatrix4x4 rotationMat;

    PosePtr selectedPose;

    QString snapshotPath;

    // Arc ball vectors for rotation with mouse
    QVector3D arcBallStartVector;
    QVector3D arcBallEndVector;

    // Arc ball vectors for translation with mouse
    QVector3D translationStartVector;
    QVector3D translationEndVector;
    QVector3D translationDifference;
    QVector3D translationStart;
    float depth = 0.f;

    // To handle dragging of the widget and clicking
    QPointF firstClickPos;
    QPointF newPos;
    QPointF currentClickPos;
    QPointF localClickPos;
    QPointF initialRenderingPosition;
    // To check whether the moved signal of the pose was because the user
    // wants to rotate or translte the pose (and therefore holds down a button)
    bool mouseDown = false;
    // To check whether the user has clicked (e.g. by adding a correspondence point)
    // or really moved the mouse while holding buttons
    bool mouseMoved = false;
    // To check whether the pose renderable has already been moved, we need this
    // to initialize some values for rotation and translation
    bool poseRenderableMoved = false;
    // This is important to check whether a pose renderable has been pressed
    // and we therefore should only move the image around when the used button
    // is not one of the buttons defined to rotate or translate a pose
    bool poseRenderablePressed = false;

    // Store the mouse button that was clicked because the Qt3D
    // pick events don't deliver them in their events when the
    // mouse is moved after clicking and holding down the mouse button
    Qt::MouseButton clickedMouseButton;

    MouseCoordinatesModificationEventFilter *mouseCoordinatesModificationEventFilter;
    bool mouseCoordinatesModificationEventFilterInstalled = false;
    QScopedPointer<QObject> eventProxy;

    QSize m_imageSize;

    SettingsPtr settings;

    static const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons> MOUSE_BUTTON_MAPPING;
};

#endif
