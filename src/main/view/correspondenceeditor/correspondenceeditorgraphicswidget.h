#ifndef CORRESPONDENCEEDITORGARPHICSVIEW_H
#define CORRESPONDENCEEDITORGARPHICSVIEW_H

#include <model/image.hpp>
#include <model/objectmodel.hpp>
#include <model/point.hpp>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class CorrespondenceEditorGraphicsWidget : public QOpenGLWidget
{
private:
    QOpenGLVertexArrayObject vertexArrayObject;
    QOpenGLBuffer buffer;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLShader *shader;
    QOpenGLTexture *texture;

public:
    CorrespondenceEditorGraphicsWidget(QWidget *parent = 0);
    ~CorrespondenceEditorGraphicsWidget();
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void setImage(Image* image);
    void setObjectModel(ObjectModel* objectModel, Point* position, Point* rotation);
};

#endif // CORRESPONDENCEEDITORGARPHICSVIEW_H
