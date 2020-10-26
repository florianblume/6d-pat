#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/rendering/clickvisualizationrenderable.hpp"
#include "qt3dwidget.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
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

class PoseViewer3DWidget : public Qt3DWidget
{
    Q_OBJECT

public:
    explicit PoseViewer3DWidget(QWidget *parent = nullptr);

    void initializeQt3D() override;

    void setBackgroundImageAndPoses(const QString& image,
                                    QMatrix3x3 cameraMatrix,
                                    QList<Pose> &poses);
    void setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix);
    void addPose(const Pose &pose);
    void updatePose(const Pose &pose);
    void removePose(const QString &id);
    void removePoses();
    PoseRenderable *getObjectModelRenderable(
            const Pose &pose);
    void setObjectsOpacity(float opacity);
    void addClick(QPoint position, QColor color);
    void removeClicks();
    void reset();
    void resizeEvent(QResizeEvent *event) override;

    ~PoseViewer3DWidget();

Q_SIGNALS:
    void positionClicked(QPoint position);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // Root entity
    Qt3DCore::QEntity *root;

    // Base framegraph
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector;
    Qt3DRender::QViewport *viewport;

    // First branch - clear the buffers
    Qt3DRender::QClearBuffers *clearBuffers;
    Qt3DRender::QNoDraw *noDraw;

    // Same camera for background image and click visualization overlay
    Qt3DRender::QCamera *planeCamera;

    // Background branch
    Qt3DRender::QLayerFilter *backgroundLayerFilter;
    Qt3DRender::QLayer *backgroundLayer;
    Qt3DRender::QCameraSelector *backgroundCameraSelector;
    Qt3DRender::QNoDepthMask *backgroundNoDepthMask;
    BackgroundImageRenderable *backgroundImageRenderable = Q_NULLPTR;

    // Poses branch
    Qt3DRender::QDepthTest *posesDepthTest;

    // Clickoverlay branch
    Qt3DRender::QLayerFilter *clickVisualizationLayerFilter;
    Qt3DRender::QLayer *clickVisualizationLayer;
    Qt3DRender::QDepthTest *clickVisualizationDepthTest;
    Qt3DRender::QCameraSelector *clickVisualizationCameraSelector;
    ClickVisualizationRenderable *clickVisualizationRenderable;

    QList<PoseRenderable *> poseRenderables;
    QMatrix4x4 projectionMatrix;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    bool mouseDown = false;
    bool mouseMoved = false;

    float farPlane = 2000.f;
    float nearPlane = 100.f;
};

#endif
