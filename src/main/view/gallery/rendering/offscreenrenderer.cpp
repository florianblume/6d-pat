#include "offscreenrenderer.hpp"
#include "misc/global.h"
#include "view/correspondenceeditor/rendering/objectmodelrenderable.hpp"
#include <QThread>
#include <QOpenGLFunctions>
#include <QRandomGenerator>
#include <QOpenGLPaintDevice>
#include <QPainter>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1

OffscreenRenderer::OffscreenRenderer(const ObjectModel &objectModel, const QSize &size) :
    objectModel(objectModel),
    size(size) {
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(0);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setOption(QSurfaceFormat::DeprecatedFunctions);
    setupProgram();
}

void OffscreenRenderer::run() {
    context = new QOpenGLContext();
    context->setFormat(surfaceFormat);
    context->create();
    context->moveToThread(QThread::currentThread());
    surface = new QOffscreenSurface();
    surface->setFormat(context->format());
    surface->create();
    surface->moveToThread(QThread::currentThread());
    context->makeCurrent(surface);
    // Initialize the buffers and renderer
    QOpenGLFramebufferObjectFormat muliSampleFormat;
    muliSampleFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    muliSampleFormat.setSamples(NUMBER_OF_SAMPLES);
    muliSampleFormat.setTextureTarget(GL_TEXTURE_2D);
    muliSampleFormat.setInternalTextureFormat(GL_RGBA);
    renderFbo = new QOpenGLFramebufferObject(size, muliSampleFormat);
    renderFbo->bind();
    glClearColor(QRandomGenerator::global()->bounded(1.0),
                 QRandomGenerator::global()->bounded(1.0),
                 QRandomGenerator::global()->bounded(1.0),
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QOpenGLPaintDevice device(size);
    QPainter painter;
    painter.begin(&device);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    QRect drawRect = QRect(0, 0, size.width(), size.height());

    painter.setPen(QPen(Qt::white, 0));
    QFont font;
    font.setPointSize(14);
    painter.setFont(font);
    painter.drawText(drawRect, objectModel.getPath(), QTextOption(Qt::AlignCenter));

    painter.end();

    glClear(GL_DEPTH_BUFFER_BIT);

    mutex.lock();
    objectsProgram->bind();
    ObjectModelRenderable renderable(objectModel,
                                     PROGRAM_VERTEX_ATTRIBUTE,
                                     PROGRAM_NORMAL_ATTRIBUTE);

    QOpenGLVertexArrayObject::Binder vaoBinder(
                renderable.getVertexArrayObject());

    objectsProgram->setUniformValue("lightPos", QVector3D(0, 10, 70));

    modelMatrix.setToIdentity();
    viewMatrix.setToIdentity();
    viewMatrix.translate(QVector3D(0, 0, -3 * renderable.getLargestVertexValue()));
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.f, size.width() / (float) size.height(), nearPlane, farPlane);
    QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    objectsProgram->setUniformValue("projectionMatrix", modelViewProjectionMatrix);

    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    objectsProgram->setUniformValue("normalMatrix", normalMatrix);

    glDrawElements(GL_TRIANGLES, renderable.getIndicesCount(), GL_UNSIGNED_INT, 0);
    objectsProgram->release();
    context->functions()->glFlush();
    image = renderFbo->toImage();
    renderFbo->release();
    context->doneCurrent();
    mutex.unlock();
    emit imageReady();
    delete context;
    delete surface;
}

QImage OffscreenRenderer::getImage() {
    return image;
}

ObjectModel OffscreenRenderer::getObjectModel() {
    return objectModel;
}

void OffscreenRenderer::setupProgram() {
    objectsProgram = new QOpenGLShaderProgram;
    // TODO: load custom shader that writes segmentations as well for clicking
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/gallery/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/gallery/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();
}
