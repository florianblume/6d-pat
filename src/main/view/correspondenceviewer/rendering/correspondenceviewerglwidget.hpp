#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/correspondence.hpp"
#include "view/correspondenceviewer/rendering/backgroundimagerenderable.hpp"
#include "view/correspondenceviewer/rendering/correspondencerenderable.hpp"
#include "view/correspondenceviewer/rendering/clickvisualizationoverlay.hpp"

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
typedef QSharedPointer<CorrespondenceRenderable> CorrespondenceRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class CorrespondenceViewerGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
    Q_OBJECT

public:
    explicit CorrespondenceViewerGLWidget(QWidget *parent = 0);
    void setBackgroundImageAndCorrespondences(const QString& image,
                                              QMatrix3x3 cameraMatrix,
                                              QList<Correspondence> &correspondences);
    void setBackgroundImage(const QString& image, QMatrix3x3 cameraMatrix);
    void addCorrespondence(const Correspondence &correspondence);
    void updateCorrespondence(const Correspondence &correspondence);
    void removeCorrespondence(const QString &id);
    void removeCorrespondences();
    CorrespondenceRenderable *getObjectModelRenderable(
            const Correspondence &correspondence);
    void setObjectsOpacity(float opacity);
    void addClick(QPoint position, QColor color);
    void removeClicks();
    void reset();

    ~CorrespondenceViewerGLWidget();

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
    void addCorrespondence(const Correspondence &correspondence, bool update);

    void initializeBackgroundProgram();
    void initializeObjectProgram();

    // Background stuff
    BackgroundImageRenderablePtr backgroundImageRenderable;
    QOpenGLShaderProgramPtr backgroundProgram;
    QMatrix4x4 backgroundProjectionMatrix;

    QVector<CorrespondenceRenderablePtr> correspondenceRenderables;
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
