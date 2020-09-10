#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "qt3dwidget.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QMatrix4x4>

#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QNoDepthMask>

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

    // Background branch
    BackgroundImageRenderable *backgroundImageRenderable = Q_NULLPTR;
    Qt3DRender::QCamera *backgroundCamera;
    Qt3DRender::QCameraSelector *backgroundCameraSelector;
    Qt3DRender::QNoDepthMask *backgroundNoDepthMask;

    // Clickoverlay branch
    //BackgroundImageRenderable *backgroundImageRenderable = Q_NULLPTR;
    Qt3DRender::QCamera *clickoverlayCamera;
    Qt3DRender::QCameraSelector *clickoverlayCameraSelector;

    QVector<PoseRenderable *> poseRenderables;
    QMatrix4x4 projectionMatrix;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    bool mouseDown = false;
    bool mouseMoved = false;

    float farPlane = 2000.f;
    float nearPlane = 100.f;
};

#endif
