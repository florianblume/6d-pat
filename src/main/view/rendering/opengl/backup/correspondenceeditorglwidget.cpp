#include "view/rendering/opengl/correspondenceeditorglwidget.hpp"

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1

CorrespondenceEditorGLWidget::CorrespondenceEditorGLWidget(QWidget *parent)
    : QOpenGLWidget(parent) {

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    setFormat(format);
}

void CorrespondenceEditorGLWidget::setObjectModel(const ObjectModel *objectModel) {
    makeCurrent();
    objectModelRenderable.reset(new ObjectModelRenderable(*objectModel,
                                                          PROGRAM_VERTEX_ATTRIBUTE,
                                                          PROGRAM_NORMAL_ATTRIBUTE));
    //! Do not call removeclickVisualizations() as this updates the view already
    this->clickVisualizations.clear();
    doneCurrent();
    update();
}

void CorrespondenceEditorGLWidget::addClickVisualization(QVector3D position) {
    this->clickVisualizations.append(position);
}

void CorrespondenceEditorGLWidget::removeClickVisualizations() {
    this->clickVisualizations.clear();
    update();
}

CorrespondenceEditorGLWidget::~CorrespondenceEditorGLWidget()
{
    makeCurrent();
    // To invoke destructors
    objectModelRenderable.reset(0);
    removeClickVisualizations();
    doneCurrent();
}

void CorrespondenceEditorGLWidget::reset() {
    objectModelRenderable.reset(0);
    clickVisualizations.clear();
    update();
}

void CorrespondenceEditorGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    initializeObjectProgram();
}

void CorrespondenceEditorGLWidget::initializeObjectProgram() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/correspondenceeditor/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/correspondenceeditor/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}

void CorrespondenceEditorGLWidget::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    objectsProgram->bind();
    {
        if (!objectModelRenderable.isNull()) {
            projectionMatrixLoc = objectsProgram->uniformLocation("projectionMatrix");
            normalMatrixLoc = objectsProgram->uniformLocation("normalMatrix");
            lightPosLoc = objectsProgram->uniformLocation("lightPos");
            segmentationColorLoc = objectsProgram->uniformLocation("segmentationColor");

            QOpenGLVertexArrayObject::Binder vaoBinder(
                        objectModelRenderable->getVertexArrayObject());

            // Light position is fixed.
            objectsProgram->setUniformValue(lightPosLoc, QVector3D(0, 0, -50));
            objectsProgram->setUniformValue(segmentationColorLoc, segmentationColor);

            viewMatrix.setToIdentity();

            QMatrix4x4 modelMatrix = objectModelRenderable->getModelMatrix();
            QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
            QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
            modelViewProjectionMatrix = QMatrix4x4(32.8108f, 0.342561f, 0.067302f, 0.0608923f,
                                                   -0.33648f, 32.8714f, 0.104916f, 0.0949239f,
                                                   1.86676, 0.0859506, -1.09821f, -0.99362f,
                                                   -35.1525f, -0.786133f, 448.169, 595.962f);
            objectsProgram->setUniformValue(projectionMatrixLoc, modelViewProjectionMatrix);

            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, objectModelRenderable->getIndicesCount(),
                           GL_UNSIGNED_INT, 0);
        }
    }
    objectsProgram->release();
}

void CorrespondenceEditorGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        mouseMoved = true;
    }
}

void CorrespondenceEditorGLWidget::resizeEvent(QResizeEvent *e) {
    int w = this->width();
    int h = this->height();
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.f, w / (float) h, nearPlane, farPlane);
    update();
}

void CorrespondenceEditorGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseMoved) {
        makeCurrent();

        QOpenGLContext *context = QOpenGLContext::currentContext();

        QOpenGLFramebufferObjectFormat format;
        format.setSamples(0);
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        readFbo = new QOpenGLFramebufferObject(size(), format);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultFramebufferObject());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, readFbo->handle());
        context->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(),
                                                     0, 0, readFbo->width(), readFbo->height(),
                                                     GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                                                     GL_NEAREST);

        readFbo->bind(); // must rebind, otherwise it won't work!

        // Read depth
        float mouseDepth = 0;
        glReadPixels(event->pos().x(), event->pos().y(), 1, 1,
                     GL_DEPTH_COMPONENT, GL_FLOAT, &mouseDepth);
        float depth = 2.0 * mouseDepth - 1.0;
        depth = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - depth * (farPlane - nearPlane));
        qDebug() << depth;
        readFbo->release();
        delete readFbo;

        doneCurrent();
    }
    mouseMoved = false;
}
