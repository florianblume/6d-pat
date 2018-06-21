#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/correspondence.hpp"
#include "view/rendering/opengl/objectmodelrenderable.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QVector3D>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>

typedef QSharedPointer<ObjectModelRenderable> ObjectModelRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class CorrespondenceEditorGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
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

    void initializeObjectProgram();

    //! We draw into a custom render buffer to be able to
    //! draw the segmentation mask simultaneously
    QOpenGLFramebufferObject *renderBuffer = 0;
    //! We need this buffer because the render buffer has
    //! multisampling enabled, otherwise we wouldn't be
    //! able to retrieve its texture
    QOpenGLFramebufferObject *segmentationSwapBuffer;
    ObjectModelRenderablePtr objectModelRenderable;
    QOpenGLShaderProgramPtr objectsProgram;
    QOpenGLTexture *segmentationTexture = 0;
    int modelViewProjectionMatrixLoc;
    int normalMatrixLoc;
    int lightPosLoc;
    int segmentationColorLoc;
    // To detect whether the object was hit by the mouse
    QColor segmentationColor = Qt::red;
    // Matrix created from the intrinsic camera parameters
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;

    QPoint lastClicked2DPos;
    QVector3D lastClicked3DPos;
    bool mouseDown = false;
    bool mouseMoved = false;

    float farPlane = 800.f;
    float nearPlane = 1.f;
};

#endif
