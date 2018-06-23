#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/correspondence.hpp"
#include "view/correspondenceeditor/rendering/correspondenceeditorglwidget.hpp"
#include "view/correspondenceeditor/rendering/objectmodelrenderable.hpp"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QVector3D>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>

typedef QSharedPointer<ObjectModelRenderable> ObjectModelRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;
typedef QSharedPointer<QOpenGLFramebufferObject> QOpenGLFramebufferObjectPtr;

class CorrespondenceEditorGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_0
{
    Q_OBJECT

public:
    explicit CorrespondenceEditorGLWidget(QWidget *parent = 0);
    void setObjectModel(const ObjectModel *objectModel);
    void addClick(QVector3D position, QColor color);
    void removeClicks();
    void reset();

    ~CorrespondenceEditorGLWidget();

signals:
    void positionClicked(QVector3D position);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    int xRot;
    int yRot;
    int zRot;

    void initializePrograms();
    void drawObject();
    void renderObjectAndSegmentation();
    QVector3D renderObjectCoordinates(QPoint point);

    ObjectModelRenderablePtr objectModelRenderable;
    QOpenGLShaderProgramPtr objectsProgram;

    QOpenGLShaderProgramPtr objectCoordsProgram;
    // The FBO to store the object coordinates for clicking
    QOpenGLFramebufferObjectPtr objectCoordsFbo;

    int segmentationColorLoc;
    // To detect whether the object was hit by the mouse
    QColor segmentationColor = QColor(255.0, 255.0, 255.0, 255.0);
    QColor segmentationBackgroundColor = QColor(0.0, 0.0, 0.0, 255.0);
    QImage renderedSegmentationImage;

    // Matrix created from the intrinsic camera parameters
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;

    QPoint lastClicked2DPos;
    QVector<QVector3D> clicks3D;
    QVector<QVector3D> colorsOfClicks;
    bool mouseDown = false;
    bool mouseMoved = false;

    float farPlane = 800.f;
    float nearPlane = 20.f;
};

#endif
