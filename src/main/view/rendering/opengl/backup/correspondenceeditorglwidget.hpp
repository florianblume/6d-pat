#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/opengl/objectmodelrenderable.h"

#include <QVector3D>
#include <QVector>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>

typedef QSharedPointer<ObjectModelRenderable> ObjectModelRenderablePtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class CorrespondenceEditorGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit CorrespondenceEditorGLWidget(QWidget *parent = 0);
    void setObjectModel(const ObjectModel *objectModel);
    void addClickVisualization(QVector3D position);
    void removeClickVisualizations();
    void reset();

    ~CorrespondenceEditorGLWidget();

signals:
    void objectModelClicked(QVector3D position);

protected:
    void initializeGL() override;
    void paintGL() override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    void initializeObjectProgram();

    QOpenGLFramebufferObject *readFbo=0;

    ObjectModelRenderablePtr objectModelRenderable;
    QOpenGLShaderProgramPtr objectsProgram;
    int projectionMatrixLoc;
    int normalMatrixLoc;
    int lightPosLoc;
    int segmentationColorLoc;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QVector<QVector3D> clickVisualizations;
    QVector3D segmentationColor = QVector3D(1.f, 1.f, 1.f);

    float farPlane = 2000.f;
    float nearPlane = 100.f;

    bool mouseMoved = false;
};

#endif
