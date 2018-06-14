
#include "view/rendering/opengl/correspondenceeditorglwidget.hpp"

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
    : QOpenGLWidget(parent) {
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
            QMatrix4x4 modelViewMatrix;
            modelViewMatrix.setToIdentity();
            int w = width();
            int h = height();
            modelViewMatrix.translate(QVector3D(0, 0, -100));
            projectionMatrix.setToIdentity();
            projectionMatrix.perspective(45.f, width() / (float) height(), 1.f, 500.f);
            QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
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

            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, renderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
    objectsProgram->release();
}

void CorrespondenceEditorGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->globalPos() - QPoint(geometry().x(), geometry().y());
}

void CorrespondenceEditorGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint newPosition = event->globalPos();
        newPosition.setX(newPosition.x() - lastPos.x());
        newPosition.setY(newPosition.y() - lastPos.y());
        move(newPosition);
        mouseMoved = true;
    }
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
