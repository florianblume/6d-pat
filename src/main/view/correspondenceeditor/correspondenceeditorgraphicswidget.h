#ifndef CORRESPONDENCEEDITORGARPHICSVIEW_H
#define CORRESPONDENCEEDITORGARPHICSVIEW_H

#include <model/image.hpp>
#include <model/objectmodel.hpp>
#include <model/point.hpp>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture);

class CorrespondenceEditorGraphicsWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    QOpenGLBuffer buffer;
    QOpenGLShaderProgram *program;
    QOpenGLTexture *imageTexture;
    QColor clearColor;
    Image* image;

    void createImageVertices();

public:
    CorrespondenceEditorGraphicsWidget(QWidget *parent = 0);
    ~CorrespondenceEditorGraphicsWidget();
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void setImage(Image* imageTexture);
    void setObjectModel(ObjectModel* objectModel, Point* position, Point* rotation);
};

#endif // CORRESPONDENCEEDITORGARPHICSVIEW_H
