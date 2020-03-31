#include "offscreenrenderer.hpp"
#include "misc/global.h"
#include "view/poseeditor/rendering/objectmodelrenderable.hpp"
#include <QThread>
#include <QOpenGLFunctions>
#include <QGuiApplication>
#include <QDebug>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_NORMAL_ATTRIBUTE 1

OffscreenRenderer::OffscreenRenderer(const QList<ObjectModel> &objectModels, const QSize &size) :
    objectModels(objectModels),
    size(size) {
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(0);
    surfaceFormat.setDepthBufferSize(DEPTH_BUFFER_SIZE);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setOption(QSurfaceFormat::DeprecatedFunctions);
}

void OffscreenRenderer::render() {
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
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/gallery/object.vert");
    objectsProgram->addShaderFromSourceFile(
                QOpenGLShader::Fragment, ":/shaders/gallery/object.frag");
    objectsProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    objectsProgram->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    objectsProgram->link();

    QOpenGLFramebufferObjectFormat muliSampleFormat;
    muliSampleFormat.setAttachment(QOpenGLFramebufferObject::Depth);
    muliSampleFormat.setSamples(NUMBER_OF_SAMPLES);
    muliSampleFormat.setTextureTarget(GL_TEXTURE_2D);
    muliSampleFormat.setInternalTextureFormat(GL_RGB);

    // Initialize the buffer
    renderFbo = new QOpenGLFramebufferObject(size, muliSampleFormat);

    for (int i = 0; i < objectModels.size() && running == 1; i++) {
        renderFbo->bind();
        objectsProgram->bind();
        ObjectModel objectModel = objectModels[i];
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

        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, renderable.getIndicesCount(), GL_UNSIGNED_INT, 0);
        context->functions()->glFlush();
        QImage image = renderFbo->toImage();
        Q_EMIT imageRendered(objectModel.getPath(), i, image);
        objectsProgram->release();
        renderFbo->release();
    }
    shutdown();
    Q_EMIT renderingFinished();
}

void OffscreenRenderer::stop() {
    // exits the rendering loop when starting to render the next object
    running = 0;
}

void OffscreenRenderer::shutdown() {
    context->makeCurrent(surface);
    delete objectsProgram;
    delete renderFbo;
    context->doneCurrent();
    delete context;
    // schedule this to be deleted only after we're done cleaning up
    surface->deleteLater();
}
