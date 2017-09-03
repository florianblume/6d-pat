#ifndef CORRESPONDENCEEDITORGARPHICSVIEW_H
#define CORRESPONDENCEEDITORGARPHICSVIEW_H

#include <model/image.hpp>
#include <model/objectmodel.hpp>
#include <model/point.hpp>
#include <model/objectimagecorrespondence.hpp>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QPoint>
#include <QMatrix4x4>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class CorrespondenceEditorGraphicsWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
private:

    QMatrix4x4 pMatrix;
    QOpenGLShaderProgram shaderProgram;
    QVector<QVector3D> imageVertices;

    QVector<QVector2D> imageTextureCoordinates;
    QOpenGLTexture* imageTexture;
    bool imageSet = false;

    QVector3D globalTranslation = {0.f, 0.f, 0.f};

    double alpha;
    double beta;
    double distance;
    QPoint lastMousePosition;
    bool mouseDown = false;
    QList<ObjectImageCorrespondence*> correspondences;

    QVector<QVector3D> createImageVertices();
    QVector<QVector3D> createObjectModelVertices(ObjectModel* objectModel);

public:
    CorrespondenceEditorGraphicsWidget(QWidget *parent = 0);
    ~CorrespondenceEditorGraphicsWidget();
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void setImage(Image* imageTexture);
    void addObjectModel(ObjectModel* objectModel, Point* position, Point* rotation);
    void updateObjectModel(ObjectModel* objectModel, Point* position, Point* rotation);
    void removeObjectModel(ObjectModel* objectModel);
    void setZoomFactor(float zoomFactor);
};

#endif // CORRESPONDENCEEDITORGARPHICSVIEW_H
