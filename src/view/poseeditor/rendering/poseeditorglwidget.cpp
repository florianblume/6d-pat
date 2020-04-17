#include "view/poseeditor/rendering/poseeditorglwidget.hpp"
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

PoseEditorGLWidget::PoseEditorGLWidget(QWidget *parent)
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

void PoseEditorGLWidget::setObjectModel(const ObjectModel *objectModel) {
    makeCurrent();
    objectModelRenderable.reset(new ObjectModelRenderable(*objectModel,
                                                          PROGRAM_VERTEX_ATTRIBUTE,
                                                          PROGRAM_NORMAL_ATTRIBUTE));
    doneCurrent();
    xTrans = 0;
    yTrans = 0;
    zTrans = 0;
    update();
}

void PoseEditorGLWidget::setRotationOfObjectModel(QVector3D rotation) {
    if (!objectModelRenderable.isNull()) {
        int x = rotation.x() * 16;
        int y = rotation.y() * 16;
        int z = rotation.z() * 16;
        setXRotation(x);
        setYRotation(y);
        setZRotation(z);
        update();
    }
}

PoseEditorGLWidget::~PoseEditorGLWidget()
{
    makeCurrent();
    // To invoke destructors
    objectModelRenderable.reset();
    objectCoordsFbo.reset();
    objectCoordsProgram.reset();
    objectsProgram.reset();
    doneCurrent();
}

static void qNormalizeAngle(int &angle) {
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void PoseEditorGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit rotationXChanged(xRot / 16.f);
        update();
    }
}

void PoseEditorGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit rotationYChanged(yRot / 16.f);
        update();
    }
}

void PoseEditorGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit rotationZChanged(zRot / 16.f);
        update();
    }
}

void PoseEditorGLWidget::addClick(QVector3D position, QColor color) {
    clicks3D.append(position);
    clickColors.append(QVector3D(color.red() / 255.f,
                               color.green() / 255.f,
                               color.blue() / 255.f));
    update();
}

void PoseEditorGLWidget::removeClicks() {
    clicks3D.clear();
    update();
}

void PoseEditorGLWidget::reset() {
    removeClicks();
    objectModelRenderable.reset();
    xTrans = 0;
    yTrans = 0;
    zTrans = 0;
    update();
}

void PoseEditorGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
    glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);

    initializePrograms();
}

void PoseEditorGLWidget::initializePrograms() {
    // Init objects shader program
    objectsProgram.reset(new QOpenGLShaderProgram);
    // TODO: load custom shader that writes segmentations as well for clicking
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/poseeditor/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/poseeditor/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();

    // Init objects shader program
    objectCoordsProgram.reset(new QOpenGLShaderProgram);
    // TODO: load custom shader that writes segmentations as well for clicking
    objectCoordsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/poseeditor/objectcoords.vert");
    objectCoordsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/poseeditor/objectcoords.frag");
    objectCoordsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectCoordsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectCoordsProgram->link();
}

void PoseEditorGLWidget::drawObject() {
    QOpenGLVertexArrayObject::Binder vaoBinder(
                objectModelRenderable->getVertexArrayObject());

    modelMatrix.setToIdentity();
    modelMatrix.rotate(180.0f - (xRot / 16.0f), 1, 0, 0);
    modelMatrix.rotate(yRot / 16.0f, 0, 1, 0);
    modelMatrix.rotate(zRot / 16.0f, 0, 0, 1);
    viewMatrix.setToIdentity();
    viewMatrix.translate(QVector3D(xTrans,
                                   yTrans,
                                   -4 * objectModelRenderable->getLargestVertexValue() + zTrans));
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.f, width() / (float) height(), nearPlane, farPlane);
    QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    objectsProgram->setUniformValue("projectionMatrix", modelViewProjectionMatrix);

    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    objectsProgram->setUniformValue("normalMatrix", normalMatrix);

    glDrawElements(GL_TRIANGLES, objectModelRenderable->getIndicesCount(), GL_UNSIGNED_INT, 0);
}

void PoseEditorGLWidget::renderObjectAndSegmentation() {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
    format.setSamples(NUMBER_OF_SAMPLES);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA);
    QOpenGLFramebufferObject fbo(width(), height(), format);
    fbo.addColorAttachment(size());
    fbo.addColorAttachment(size());
    fbo.bind();

    // Clear buffers.
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    GLenum bufs[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    f->glDrawBuffers(3, bufs);

    GLfloat objectBackground[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    f->glClearBufferfv(GL_COLOR, 0, objectBackground);

    GLfloat otherBackground[4] = { (float) segmentationBackgroundColor.red(),
                                   (float) segmentationBackgroundColor.green(),
                                   (float) segmentationBackgroundColor.blue(),
                                   (float) segmentationBackgroundColor.alpha(),};
    f->glClearBufferfv(GL_COLOR, 1, otherBackground);
    f->glClearBufferfv(GL_COLOR, 2, otherBackground);

    glClear(GL_DEPTH_BUFFER_BIT);

    if (!objectModelRenderable.isNull()) {
        objectsProgram->bind();
        {
            // Light position is fixed.
            objectsProgram->setUniformValue("lightPos", QVector3D(0, 10, 70));
            objectsProgram->setUniformValue("segmentationColor", segmentationColor);
            objectsProgram->setUniformValueArray("clickPositions",
                                                 clicks3D.constData(),
                                                 clicks3D.size());
            objectsProgram->setUniformValueArray("clickColors",
                                                 clickColors.constData(),
                                                 clickColors.size());
            objectsProgram->setUniformValue("clickCount", clicks3D.size());
            objectsProgram->setUniformValue("circumfence",
                                            objectModelRenderable->getLargestVertexValue() / 70.f);
            drawObject();
        }
        objectsProgram->release();

        renderedSegmentationImage = fbo.toImage(true, 1);
    }

    fbo.release();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo.handle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject());
    f->glBlitFramebuffer(0, 0, width(), height(),
                                                 0, 0, width(), height(),
                                                 GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                                                 GL_NEAREST);
}

QVector3D PoseEditorGLWidget::renderObjectCoordinates(QPoint point) {
    makeCurrent();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
    // Segmentation mask gets multisampled already, we don't need to again here
    format.setSamples(0);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA32F);
    objectCoordsFbo.reset(new QOpenGLFramebufferObject(width(), height(), format));

    objectCoordsFbo->bind();

    // Clear buffers.
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
    f->glDrawBuffers(1, bufs);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float pixel[3];

    if (!objectModelRenderable.isNull()) {
        objectCoordsProgram->bind();
        {
            drawObject();
        }
        objectCoordsProgram->release();
        glReadPixels( point.x(), height() - point.y(), 1, 1, GL_RGB,  GL_FLOAT, &pixel );
    }
    objectCoordsFbo->release();
    objectCoordsFbo.reset();
    doneCurrent();
    return QVector3D(pixel[0], pixel[1], pixel[2]);
}

void PoseEditorGLWidget::paintGL() {
    renderObjectAndSegmentation();
}

void PoseEditorGLWidget::mousePressEvent(QMouseEvent *event) {
    lastClicked2DPos = event->pos();
}

void PoseEditorGLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastClicked2DPos.x();
    int dy = lastClicked2DPos.y() - event->y();
    int d = 0;
    if (qAbs(dx) > qAbs(dy))
        d = dx;
    else
        d = dy;

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * d);
        mouseMoved = true;
    } else if (event->buttons() & Qt::RightButton) {
        setZRotation(zRot + 8 * d);
        mouseMoved = true;
    }else if (event->buttons() & Qt::MiddleButton) {
        setYRotation(yRot + 8 * d);
        mouseMoved = true;
    }
    lastClicked2DPos = event->pos();
}

void PoseEditorGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mouseMoved && !objectModelRenderable.isNull()) {
        QPoint mousePos = event->pos();
        QColor mouseClickColor =
                renderedSegmentationImage.pixelColor(mousePos.x(), mousePos.y());
        if (mouseClickColor == segmentationColor) {
            QVector3D pos3D = renderObjectCoordinates(mousePos);
            Q_EMIT positionClicked(pos3D);
        }
    }
    mouseMoved = false;
}

void PoseEditorGLWidget::keyPressEvent(QKeyEvent *ev) {
    if (ev->key() == Qt::Key_Shift) {
        shiftKeyPressed = true;
    } else if (ev->key() == Qt::Key_Up ||
               ev->key() == Qt::Key_Down ||
               ev->key() == Qt::Key_Right ||
               ev->key() == Qt::Key_Left) {
        if (shiftKeyPressed) {
            if (ev->key() == Qt::Key_Up)
                shiftDirectionZ = 1;
            else if (ev->key() == Qt::Key_Down)
                shiftDirectionZ = -1;
            else if (ev->key() == Qt::Key_Left)
                shiftDirectionX = -1;
            else if (ev->key() == Qt::Key_Right)
                shiftDirectionX = 1;
        } else {
            if (ev->key() == Qt::Key_Up)
                shiftDirectionY = 1;
            else if (ev->key() == Qt::Key_Down)
                shiftDirectionY = -1;
            else if (ev->key() == Qt::Key_Left)
                shiftDirectionX = -1;
            else if (ev->key() == Qt::Key_Right)
                shiftDirectionX = 1;
        }
        keyPressedTimer.reset(new QTimer());
        connect(keyPressedTimer.data(), &QTimer::timeout,
                this, &PoseEditorGLWidget::updateCameraPosition);
        keyPressedTimer->start(10);
    }

}

void PoseEditorGLWidget::updateCameraPosition() {
    if (!objectModelRenderable.isNull()) {
        float factor = objectModelRenderable->getLargestVertexValue() / 10.f;
        xTrans += shiftDirectionX * factor;
        yTrans += shiftDirectionY * factor;
        zTrans += shiftDirectionZ * factor;
        update();
    }
}

void PoseEditorGLWidget::keyReleaseEvent(QKeyEvent *ev) {
    switch(ev->key()) {
        case Qt::Key_Up: shiftDirectionY = 0; shiftDirectionZ = 0; break;
        case Qt::Key_Down: shiftDirectionY = 0; shiftDirectionZ = 0; break;
        case Qt::Key_Left: shiftDirectionX = 0; break;
        case Qt::Key_Right: shiftDirectionX = 0; break;
    }

    if (ev->key() == Qt::Key_Shift) {
        shiftKeyPressed = false;
    } else if (!keyPressedTimer.isNull()) {
        keyPressedTimer->stop();
        keyPressedTimer.reset();
    }
}
