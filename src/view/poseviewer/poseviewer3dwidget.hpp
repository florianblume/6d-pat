#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "qt3dwidget.h"
#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
#include "settings/settings.hpp"

#include <QString>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QList>
#include <QMatrix4x4>
#include <QResizeEvent>

#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QMultiSampleAntiAliasing>

class PoseViewer3DWidget : public Qt3DWidget
{
    Q_OBJECT

public:
    explicit PoseViewer3DWidget(QWidget *parent = nullptr);
    ~PoseViewer3DWidget();

    void initializeQt3D() override;

    void setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix);
    void setPoses(const QList<PosePtr> &poses);
    void addPose(PosePtr pose);
    void removePose(PosePtr pose);

    void selectPose(PosePtr selected, PosePtr deselected);
    void setObjectsOpacity(float opacity);
    void setClicks(const QList<QPoint> &clicks);
    QSize imageSize() const;
    void reset();
    void setSettings(SettingsPtr settings);
    void resizeEvent(QResizeEvent *event) override;

public Q_SLOTS:
    // React to object pickers
    void onPoseRenderableMoved(Qt3DRender::QPickEvent *pickEvent);
    void onPoseRenderablePressed(Qt3DRender::QPickEvent *pickEvent);
    void onBackgroundImageRenderableMoved(Qt3DRender::QPickEvent *pickEvent);
    void onBackgroundImageRenderableClicked(Qt3DRender::QPickEvent *pickEvent);
    void onBackgroundImageRenderablePressed(Qt3DRender::QPickEvent *pickEvent);

Q_SIGNALS:
    void positionClicked(QPoint position);
    void poseSelected(PosePtr pose);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector3D arcBallVectorForMousePos(const QPointF pos);

private:
    // Root entity
    Qt3DCore::QEntity *root;

    // Base framegraph
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector;
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
    QPointer<BackgroundImageRenderable> backgroundImageRenderable;

    // Poses branch
    Qt3DRender::QLayerFilter *posesLayerFilter;
    Qt3DRender::QLayer *posesLayer;
    Qt3DRender::QCamera *posesCamera;
    Qt3DRender::QCameraSelector *posesCameraSelector;
    Qt3DRender::QDepthTest *posesDepthTest;
    Qt3DRender::QMultiSampleAntiAliasing *posesMultiSampling;

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

    QMatrix4x4 rotationMat;

    PosePtr selectedPose;

    // Arc ball vectors for rotation with mouse
    QVector3D arcBallStartVector;
    QVector3D arcBallEndVector;

    // Arc ball vectors for translation with mouse
    QVector3D translationStartVector;
    QVector3D translationEndVector;
    QVector3D translationDifference;
    QVector3D translationStart;
    float depth = 0.f;
    float distance = 0.f;

    // To handle dragging of the widget and clicking
    QPoint firstClickPos;
    QPoint newPos;
    QPoint currentClickPos;
    QPointF localClickPos;
    bool mouseDown = false;
    bool mouseMoved = false;
    bool backgroundImageRenderableMovedFirst = false;
    bool poseRenderableMovedFirst = false;

    // Store the mouse button that was clicked because the Qt3D
    // pick events don't deliver them in their events when the
    // mouse is moved after clicking and holding down the mouse button
    Qt::MouseButton clickedMouseButton;

    QSize m_imageSize;

    // TODO Read from file
    float farPlane = 2000.f;
    float nearPlane = 100.f;

    float oldDepth = -1.f;

    SettingsPtr settings;

    static const QMap<Qt::MouseButton, Qt3DRender::QPickEvent::Buttons> MOUSE_BUTTON_MAPPING;
};

#endif
