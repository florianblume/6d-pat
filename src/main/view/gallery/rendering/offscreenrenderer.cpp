#include "offscreenrenderer.hpp"
#include "misc/global.h"
#include "view/poseeditor/rendering/objectmodelrenderable.hpp"
#include <QThread>
#include <QOpenGLFunctions>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1

OffscreenRenderer::OffscreenRenderer(const ObjectModel &objectModel, const QSize &size) :
    objectModel(objectModel),
    size(size) {
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(0);
    surfaceFormat.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setOption(QSurfaceFormat::DeprecatedFunctions);
}

void OffscreenRenderer::run() {
    context = new QOpenGLContext();
    context->setFormat(surfaceFormat);
    context->create();
    //context->moveToThread(QThread::currentThread());
    surface = new QOffscreenSurface();
    surface->setFormat(context->format());
    surface->create();
    //surface->moveToThread(QThread::currentThread());
    context->makeCurrent(surface);

    objectsProgram = new QOpenGLShaderProgram;
    // TODO: load custom shader that writes segmentations as well for clicking
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/gallery/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/gallery/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();

    // Initialize the buffers and renderer
    QOpenGLFramebufferObjectFormat muliSampleFormat;
    muliSampleFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    muliSampleFormat.setSamples(NUMBER_OF_SAMPLES);
    muliSampleFormat.setTextureTarget(GL_TEXTURE_2D);
    muliSampleFormat.setInternalTextureFormat(GL_RGBA);
    renderFbo = new QOpenGLFramebufferObject(size, muliSampleFormat);
    renderFbo->bind();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    objectsProgram->bind();
    ObjectModelRenderable renderable(objectModel,
                                     PROGRAM_VERTEX_ATTRIBUTE,
                                     PROGRAM_NORMAL_ATTRIBUTE);

    QOpenGLVertexArrayObject::Binder vaoBinder(
                renderable.getVertexArrayObject());

    objectsProgram->setUniformValue("lightPos", QVector3D(0, 0, 2 * renderable.getLargestVertexValue()));

    modelMatrix.setToIdentity();
    modelMatrix.rotate(90.0, QVector3D(1.0, 0.0, 0.0));
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
    Q_EMIT imageReady();
    delete context;
    delete surface;
    delete objectsProgram;
    delete renderFbo;
}

QImage OffscreenRenderer::getImage() {
    return image;
}

ObjectModel OffscreenRenderer::getObjectModel() {
    return objectModel;
}
