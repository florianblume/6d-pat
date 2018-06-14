
#include "view/rendering/opengl/correspondenceeditorglwidget.hpp"

#include <cmath>
#include <QFrame>
#include <QImage>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include <QMouseEvent>
#include <QThread>
#include <QApplication>
#include <QPainter>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 1

CorrespondenceEditorGLWidget::CorrespondenceEditorGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      xRot(0),
      yRot(0),
      zRot(0) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    //timer->start(1000);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    setFormat(format);

    clickOverlay = new ClickVisualizationOverlay(this);
    clickOverlay->resize(this->size());
}

CorrespondenceEditorGLWidget::~CorrespondenceEditorGLWidget()
{
    makeCurrent();
    // To invoke destructors
    removeCorrespondences();
    doneCurrent();
}

static void qNormalizeAngle(int &angle) {
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void CorrespondenceEditorGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        update();
    }
}

void CorrespondenceEditorGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        update();
    }
}

void CorrespondenceEditorGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        update();
    }
}

void CorrespondenceEditorGLWidget::addCorrespondence(const Correspondence &correspondence) {
    addCorrespondence(correspondence, true);
}

void CorrespondenceEditorGLWidget::updateCorrespondence(const Correspondence &correspondence) {
    CorrespondenceRenderable *renderable = getObjectModelRenderable(correspondence);
    renderable->setPosition(correspondence.getPosition());
    renderable->setRotation(correspondence.getRotation());
    update();
}

void CorrespondenceEditorGLWidget::removeCorrespondence(const Correspondence &correspondence) {
    for (uint index = 0; index < objectModelRenderables.size(); index++) {
        if (objectModelRenderables[index]->getCorrespondenceId() == correspondence.getID()) {
            objectModelRenderables.remove(index);
            break;
        }
    }
    update();
}

void CorrespondenceEditorGLWidget::removeCorrespondences() {
    objectModelRenderables.clear();
    update();
}

CorrespondenceRenderable *CorrespondenceEditorGLWidget::getObjectModelRenderable(const Correspondence &correspondence) {
    for (ObjectModelRenderablePtr &ptr : objectModelRenderables) {
        if (ptr->getCorrespondenceId() == correspondence.getID()) {
            return ptr.data();
        }
    }
}

void CorrespondenceEditorGLWidget::setObjectsOpacity(float opacity) {
    this->opacity = opacity;
    update();
}

void CorrespondenceEditorGLWidget::addClick(QPoint position, QColor color) {
    clickOverlay->addClickedPoint(position, color);
}

void CorrespondenceEditorGLWidget::removeClicks() {
    clickOverlay->removeClickedPoints();
}

void CorrespondenceEditorGLWidget::reset() {
    removeClicks();
    removeCorrespondences();
}

void CorrespondenceEditorGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    initializeObjectProgram();
}

void CorrespondenceEditorGLWidget::initializeObjectProgram() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/correspondenceviewer/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/correspondenceviewer/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}

void CorrespondenceEditorGLWidget::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    objectsProgram->bind();
    {
        projectionMatrixLoc = objectsProgram->uniformLocation("projectionMatrix");
        normalMatrixLoc = objectsProgram->uniformLocation("normalMatrix");
        lightPosLoc = objectsProgram->uniformLocation("lightPos");
        opacityLoc = objectsProgram->uniformLocation("opacity");

        for (ObjectModelRenderablePtr &renderable : objectModelRenderables) {
            QOpenGLVertexArrayObject::Binder vaoBinder(renderable->getVertexArrayObject());

            // Light position is fixed.
            objectsProgram->setUniformValue(lightPosLoc, QVector3D(0, 10, 70));
            objectsProgram->setUniformValue(opacityLoc, opacity);

            // Compute the projection matrix that includes the intrinsic camera parameters
            // as well as the translation and rotation of the object
            //QMatrix4x4 modelViewMatrix = renderable->getModelViewMatrix();
            QMatrix4x4 modelMatrix;
            modelMatrix.setToIdentity();
            modelMatrix.rotate(180.0f - (xRot / 16.0f), 1, 0, 0);
            modelMatrix.rotate(yRot / 16.0f, 0, 1, 0);
            modelMatrix.rotate(zRot / 16.0f, 0, 0, 1);
            QMatrix4x4 viewMatrix;
            viewMatrix.translate(QVector3D(0, 0, -100));
            projectionMatrix.setToIdentity();
            projectionMatrix.perspective(45.f, width() / (float) height(), 1.f, 500.f);
            QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
            //modelViewProjectionMatrix = modelViewProjectionMatrix.transposed();
            /*
            modelViewProjectionMatrix = QMatrix4x4(
                        2.13295f,
                        0.f,
                        0.0f,
                        0.0f,
                        0.f,
                        2.41421f,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f,
                        -1.2222f,
                        -1.0f,
                        0.0f,
                        0.0f,
                        5.f,
                        20.f);
                        */
            objectsProgram->setUniformValue(projectionMatrixLoc, modelViewProjectionMatrix);

            QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, renderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
    objectsProgram->release();
}

void CorrespondenceEditorGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void CorrespondenceEditorGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}

void CorrespondenceEditorGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseMoved) {
        emit positionClicked(event->pos());
    }
    mouseMoved = false;
}

void CorrespondenceEditorGLWidget::addCorrespondence(const Correspondence &correspondence,
                                                     bool update) {
    makeCurrent();
    ObjectModelRenderablePtr renderable(new CorrespondenceRenderable(correspondence,
                                                                  PROGRAM_VERTEX_ATTRIBUTE,
                                                                  PROGRAM_NORMAL_ATTRIBUTE));
    objectModelRenderables.append(renderable);
    doneCurrent();
    if (update)
        this->update();
}
