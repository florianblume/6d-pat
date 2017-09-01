#include "correspondenceeditorgraphicswidget.h"
#include <QOpenGLFunctions>

CorrespondenceEditorGraphicsWidget::CorrespondenceEditorGraphicsWidget(QWidget *parent) : QOpenGLWidget (parent),
    shaderProgram(0), shader(0), texture(0) {
}

CorrespondenceEditorGraphicsWidget::~CorrespondenceEditorGraphicsWidget() {
    makeCurrent();

    delete texture;
    delete shader;
    delete shaderProgram;

    vertexArrayObject.destroy();
    buffer.destroy();

    doneCurrent();
}

void CorrespondenceEditorGraphicsWidget::initializeGL() {
    /*
    vertexArrayObject.create();
    if (vertexArrayObject.isCreated())
        vertexArrayObject.bind();

    buffer.create();
    buffer.bind();
    */

    //texture = new QOpenGLTexture(QImage());
}

void CorrespondenceEditorGraphicsWidget::resizeGL(int w, int h) {

}

void CorrespondenceEditorGraphicsWidget::paintGL() {
    /*
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    //matrix.rotate(rotation);

    // Set modelview-projection matrix
    shaderProgram->setUniformValue("mvp_matrix", matrix);

    // Use texture unit 0 which contains cube.png
    shaderProgram->setUniformValue("texture", 0);

    // Draw cube geometry
    //geometries->drawCubeGeometry(&program);
    */
}

void CorrespondenceEditorGraphicsWidget::setImage(Image* image) {
    //texture->setData(QImage(image->getAbsoluteImagePath().c_str()));
    //update();
}

void CorrespondenceEditorGraphicsWidget::setObjectModel(ObjectModel* objectModel, Point* position, Point* rotation) {

}
