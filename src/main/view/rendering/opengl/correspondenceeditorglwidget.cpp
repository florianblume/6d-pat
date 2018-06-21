#include "view/rendering/opengl/correspondenceeditorglwidget.hpp"
#include "misc/global.h"

#include <cmath>
#include <QFrame>
#include <QImage>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 1

CorrespondenceEditorGLWidget::CorrespondenceEditorGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      xRot(0),
      yRot(0),
      zRot(0) {

    QSurfaceFormat format;
    format.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    format.setStencilBufferSize(STENCIL_BUFFER_SIZE);
    format.setSamples(NUMBER_OF_SAMPLES);
    setFormat(format);
}

void CorrespondenceEditorGLWidget::setObjectModel(const ObjectModel *objectModel) {
    makeCurrent();
    objectModelRenderable.reset(new ObjectModelRenderable(*objectModel,
                                                          PROGRAM_VERTEX_ATTRIBUTE,
                                                          PROGRAM_NORMAL_ATTRIBUTE));
    doneCurrent();
    update();
}

CorrespondenceEditorGLWidget::~CorrespondenceEditorGLWidget()
{
    makeCurrent();
    // To invoke destructors
    objectModelRenderable.reset();
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

void CorrespondenceEditorGLWidget::addClick(QVector3D position, QColor color) {
    // TODO: add sphere
}

void CorrespondenceEditorGLWidget::removeClicks() {
    // TODO: remove spheres
}

void CorrespondenceEditorGLWidget::reset() {
    removeClicks();
    objectModelRenderable.reset();
    update();
}

void CorrespondenceEditorGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    initializePrograms();
}

void CorrespondenceEditorGLWidget::initializePrograms() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    // TODO: load custom shader that writes segmentations as well for clicking
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/correspondenceeditor/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/correspondenceeditor/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}

void CorrespondenceEditorGLWidget::paintGL() {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
    format.setSamples(NUMBER_OF_SAMPLES);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA32F);
    QOpenGLFramebufferObject fbo(width(), height(), format);
    // Attachment for segmentation
    fbo.addColorAttachment(width(), height());
    // Attachment for object coordinates
    fbo.addColorAttachment(width(), height());
    fbo.bind();

    // Clear buffers.
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    GLenum bufs[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    f->glDrawBuffers(3, bufs);

    GLfloat objectBackground[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    f->glClearBufferfv(GL_COLOR, 0, objectBackground);

    GLfloat otherBackground[4] = { segmentationBackgroundColor.red(),
                                 segmentationBackgroundColor.green(),
                                 segmentationBackgroundColor.blue(),
                                 segmentationBackgroundColor.alpha(),};
    f->glClearBufferfv(GL_COLOR, 1, otherBackground);
    f->glClearBufferfv(GL_COLOR, 2, otherBackground);

    glClear(GL_DEPTH_BUFFER_BIT);

    if (!objectModelRenderable.isNull()) {
        objectsProgram->bind();
        {
            modelViewProjectionMatrixLoc = objectsProgram->uniformLocation("projectionMatrix");
            normalMatrixLoc = objectsProgram->uniformLocation("normalMatrix");
            lightPosLoc = objectsProgram->uniformLocation("lightPos");
            segmentationColorLoc = objectsProgram->uniformLocation("segmentationColor");

            QOpenGLVertexArrayObject::Binder vaoBinder(
                        objectModelRenderable->getVertexArrayObject());

            // Light position is fixed.
            objectsProgram->setUniformValue(lightPosLoc, QVector3D(0, 10, 70));
            objectsProgram->setUniformValue(segmentationColorLoc, segmentationColor);

            modelMatrix.setToIdentity();
            modelMatrix.rotate(180.0f - (xRot / 16.0f), 1, 0, 0);
            modelMatrix.rotate(yRot / 16.0f, 0, 1, 0);
            modelMatrix.rotate(zRot / 16.0f, 0, 0, 1);
            viewMatrix.setToIdentity();
            viewMatrix.translate(QVector3D(0, 0, -4 * objectModelRenderable->getLargestVertexValue()));
            projectionMatrix.setToIdentity();
            projectionMatrix.perspective(45.f, width() / (float) height(), nearPlane, farPlane);
            QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
            objectsProgram->setUniformValue(modelViewProjectionMatrixLoc, modelViewProjectionMatrix);

            QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
            QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
            objectsProgram->setUniformValue(normalMatrixLoc, normalMatrix);

            glDrawElements(GL_TRIANGLES, objectModelRenderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
        objectsProgram->release();

        renderedSegmentationImage = fbo.toImage(true, 1);

        // Since we use multisampling we need to blit before
        // using glReadPixels
        format.setSamples(0);
        QOpenGLFramebufferObject fbo2(width(), height(), format);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo2.handle());

        QOpenGLFramebufferObject::blitFramebuffer(&fbo2,
                          QRect(0, 0, width(), height()),
                          &fbo,
                          QRect(0, 0, width(), height()),
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST,
                          2,
                          0,
                          QOpenGLFramebufferObject::DontRestoreFramebufferBinding);
        fbo2.bind();
        int size = width() * height();
        GLfloat* pixels = new GLfloat[ size * 4 ];
        glReadPixels( 0, 0, width(), height(), GL_RGBA,  GL_FLOAT, pixels );

        QByteArray data(nullptr, size * 4);

        for (int i = 0, j = 0; i < size * 4; ++i, j += 4) {
            data[j] = data[j + 1] = data[j + 2] =         // R, G, B
                (unsigned char)(pixels[i] * 255);
            qDebug() << pixels[i] << pixels[i + 1]<< pixels[i+2];

            data[j + 4] = ~0;       // Alpha
        }

        QImage image((const uchar*) data.constData(), width(), height(), QImage::Format_ARGB32_Premultiplied);
        image.save("test.png");
    }

    fbo.release();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.handle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
    f->glBlitFramebuffer(0, 0, width(), height(),
                                                 0, 0, width(), height(),
                                                 GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                                                 GL_NEAREST);
}

void CorrespondenceEditorGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastClicked2DPos = event->pos();
}

void CorrespondenceEditorGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastClicked2DPos.x();
    int dy = event->y() - lastClicked2DPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
        mouseMoved = true;
    } else if (event->buttons() & Qt::RightButton) {
        setZRotation(zRot + 8 * dx);
        mouseMoved = true;
    }
    lastClicked2DPos = event->pos();
}

void CorrespondenceEditorGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseMoved) {
        QPoint mousePos = event->pos();
        QColor mouseClickColor = renderedSegmentationImage.pixelColor(mousePos.x(), mousePos.y());
        if (mouseClickColor == segmentationColor) {
            makeCurrent();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultFramebufferObject());
            qDebug() << event->pos();
            // Read depth
            float mouseDepth = 1.f;
            glReadPixels(event->pos().x(), event->pos().y(), 1, 1,
                         GL_DEPTH_COMPONENT, GL_FLOAT, &mouseDepth);
            qDebug() << mouseDepth;
            float depth = 2.0 * mouseDepth - 1.0;
            depth = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - depth * (farPlane - nearPlane));
            qDebug() << depth;

            doneCurrent();
        }
    }
    mouseMoved = false;
        /*

        QOpenGLContext *context = QOpenGLContext::currentContext();

        QOpenGLFramebufferObjectFormat format;
        format.setSamples(0);
        segmentationBuffer = new QOpenGLFramebufferObject(size(), format);
        segmentationBuffer->addColorAttachment(size());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, renderBuffer->handle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, segmentationBuffer->handle());

        context->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(),
                                                     0, 0, renderBuffer->width(), renderBuffer->height(),
                                                     GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                                                     GL_NEAREST);

        segmentationBuffer->bind();
        QImage image = segmentationBuffer->toImage();
        image.save("test2.png");
        segmentationBuffer->release();
        delete segmentationBuffer;
        context->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(),
                                                     0, 0, swapBuffer->width(), swapBuffer->height(),
                                                     GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                                                     GL_NEAREST);

        swapBuffer->bind(); // must rebind, otherwise it won't work!
        qDebug() << event->pos();
        // Read depth
        float mouseDepth = 0;
        glReadPixels(event->pos().x(), event->pos().y(), 1, 1,
                     GL_DEPTH_COMPONENT, GL_FLOAT, &mouseDepth);
        float depth = 2.0 * mouseDepth - 1.0;
        depth = 2.0 * nearPlane * farPlane / (farPlane + nearPlane - depth * (farPlane - nearPlane));
        qDebug() << depth;
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        GLuint clickColor;
        glReadPixels(event->pos().x(), event->pos().y(), 1, 1,
                     GL_RGB, GL_UNSIGNED_BYTE, &clickColor);
        qDebug() << clickColor;
        swapBuffer->release();
        delete swapBuffer;

        doneCurrent();

    }
    mouseMoved = false;*/
}
