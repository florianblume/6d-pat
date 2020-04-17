#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/pose.hpp"
#include "view/rendering/backgroundimagerenderable.hpp"
#include "view/rendering/poserenderable.hpp"
#include "view/poseviewer/rendering/clickvisualizationoverlay.hpp"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>

typedef QSharedPointer<BackgroundImageRenderable> BackgroundImageRenderablePtr;
typedef QSharedPointer<PoseRenderable> PoseRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class PoseViewerGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
    Q_OBJECT

public:
    explicit PoseViewerGLWidget(QWidget *parent = 0);
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

    ~PoseViewerGLWidget();

Q_SIGNALS:
    void positionClicked(QPoint position);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    void setBackgroundImage(const QString& image,
                            QMatrix3x3 cameraMatrix,
                            bool update);
    void addPose(const Pose &pose, bool update);

    void initializeBackgroundProgram();
    void initializeObjectProgram();

    // Background stuff
    BackgroundImageRenderablePtr backgroundImageRenderable;
    QOpenGLShaderProgramPtr backgroundProgram;
    QMatrix4x4 backgroundProjectionMatrix;

    QVector<PoseRenderablePtr> poseRenderables;
    QOpenGLShaderProgramPtr objectsProgram;
    int projectionMatrixLoc;
    int normalMatrixLoc;
    int lightPosLoc;
    int opacityLoc;
    // Matrix created from the intrinsic camera parameters
    QMatrix4x4 projectionMatrix;
    float opacity = 1.f;

    // To handle dragging of the widget and clicking
    QPoint lastPos;
    bool mouseDown = false;
    bool mouseMoved = false;
    ClickVisualizationOverlay *clickOverlay;

    float farPlane = 2000.f;
    float nearPlane = 100.f;
};

#endif
