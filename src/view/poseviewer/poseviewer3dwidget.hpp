#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
#include "qt3dwidget.h"

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

    void setBackgroundImageAndPoses(const QString &image,
                                    const QMatrix3x3 &cameraMatrix,
                                    const QList<PosePtr> &poses);
    void setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix);
    void addPose(PosePtr pose);
    void removePose(const QString &id);
    void removePoses();
    void selectPose(PosePtr selected, PosePtr deselected);
    void onPoseRenderableMoved(Qt3DRender::QPickEvent *e);
    void onBackgroundImageRenderableMoved(Qt3DRender::QPickEvent *e);
    void onBackgroundImageRenderableClicked(Qt3DRender::QPickEvent *e);
    void setObjectsOpacity(float opacity);
    void setClicks(const QList<QPoint> &clicks);
    QSize imageSize() const;
    void reset();
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void positionClicked(QPoint position);
    void poseSelected(PosePtr pose);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector3D getArcBallVector(int x, int y);

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

    QVector3D startVector;
    QVector3D endVector;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    QPoint newPos;
    QPoint clickPos;
    QPointF localClickPos;
    bool mouseDown = false;
    bool mouseMoved = false;
    bool mouseDownOnBackground = false;
    bool poseRenderableMoved = false;

    QSize m_imageSize;

    // TODO Read from file
    float farPlane = 2000.f;
    float nearPlane = 100.f;

    float oldDepth = -1.f;
};

#endif
