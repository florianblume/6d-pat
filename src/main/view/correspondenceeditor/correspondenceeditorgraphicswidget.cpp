#include "correspondenceeditorgraphicswidget.h"
#include "misc/otiathelper.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#ifdef WIN32
    #include <GL/glext.h>
    PFNGLACTIVETEXTUREPROC pGlActiveTexture = NULL;
    #define glActiveTexture pGlActiveTexture
#endif //WIN32

CorrespondenceEditorGraphicsWidget::CorrespondenceEditorGraphicsWidget(QWidget *parent)
    : QOpenGLWidget (parent) {
    alpha = 25;
    beta = -25;
    distance = 2.8;
}

CorrespondenceEditorGraphicsWidget::~CorrespondenceEditorGraphicsWidget() {
    makeCurrent();
    doneCurrent();
}

void CorrespondenceEditorGraphicsWidget::initializeGL() {
    initializeOpenGLFunctions();
    //! [1]
    //! [2]
    #ifdef WIN32
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress((LPCSTR) "glActiveTexture");
    #endif
    //! [2]

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    //QSurfaceFormat::defaultFormat().setSamples(4);

    glClearColor(255, 255, 255, 0);

    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "/home/flo/git/Otiat/src/main/view/correspondenceeditor/vertexShader.vsh");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "/home/flo/git/Otiat/src/main/view/correspondenceeditor/fragmentShader.fsh");
    shaderProgram.link();

    imageVertices << createImageVertices();
    //! [3]
    imageTextureCoordinates << QVector2D(0, 0) << QVector2D(1, 0) << QVector2D(1, 1) // Front
                       << QVector2D(1, 1) << QVector2D(0, 1) << QVector2D(0, 0);
}

void CorrespondenceEditorGraphicsWidget::resizeGL(int w, int h) {
    if (h == 0) {
        h = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) w / (float) h, 0.0001, 1000);

    glViewport(0, 0, w, h);
}

void CorrespondenceEditorGraphicsWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (imageSet) {
        QMatrix4x4 mMatrix;
        mMatrix.translate(globalTranslation);

        QMatrix4x4 vMatrix;

        QMatrix4x4 cameraTransformation;

        QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
        QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

        vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

        shaderProgram.bind();

        shaderProgram.setUniformValue("mvpMatrix", pMatrix * vMatrix * mMatrix);

        shaderProgram.setUniformValue("texture", 0);

        imageTexture->bind();

        shaderProgram.setAttributeArray("vertex", imageVertices.constData());
        shaderProgram.enableAttributeArray("vertex");

        shaderProgram.setAttributeArray("textureCoordinate", imageTextureCoordinates.constData());
        shaderProgram.enableAttributeArray("textureCoordinate");

        glDrawArrays(GL_TRIANGLES, 0, imageVertices.size());

        shaderProgram.disableAttributeArray("vertex");

        shaderProgram.disableAttributeArray("textureCoordinate");

        shaderProgram.release();
    }
}

void CorrespondenceEditorGraphicsWidget::mousePressEvent(QMouseEvent *event) {
    mouseDown = true;
    lastMousePosition = event->pos();
}

void CorrespondenceEditorGraphicsWidget::mouseMoveEvent(QMouseEvent *event) {
    if (mouseDown) {
        QPoint newPosition = event->pos();
        int differenceX = newPosition.x() - lastMousePosition.x();
        int differenceY = newPosition.y() - lastMousePosition.y();
        globalTranslation.setX(globalTranslation.x() + differenceX * 0.003201f);
        globalTranslation.setY(globalTranslation.y() - differenceY * 0.003201f);
        lastMousePosition = newPosition;
        update();
    }
}

void CorrespondenceEditorGraphicsWidget::mouseReleaseEvent(QMouseEvent *event) {
    mouseDown = false;
}

void CorrespondenceEditorGraphicsWidget::setImage(Image* image) {
    if (image) {
        if (imageSet) {
            delete imageTexture;
        }
        imageTexture = new QOpenGLTexture(QImage(QString(image->getAbsoluteImagePath().c_str())).mirrored());
        imageSet = true;
        update();
    }
}

void CorrespondenceEditorGraphicsWidget::addObjectModel(ObjectModel* objectModel, Point* position, Point* rotation) {

}

void CorrespondenceEditorGraphicsWidget::updateObjectModel(ObjectModel* objectModel, Point* position, Point* rotation) {

}

void CorrespondenceEditorGraphicsWidget::removeObjectModel(ObjectModel* objectModel) {

}

QVector<QVector3D> CorrespondenceEditorGraphicsWidget::createImageVertices() {
    QVector<QVector3D> vertices;
    //! Create a plane that displays the selected image
    vertices << QVector3D(-1, -1,  1) << QVector3D( 1, -1,  1) << QVector3D( 1,  1,  1)
             << QVector3D( 1,  1,  1) << QVector3D(-1,  1,  1) << QVector3D(-1, -1,  1);
    return vertices;
}

void CorrespondenceEditorGraphicsWidget::setZoomFactor(float zoomFactor) {

}
