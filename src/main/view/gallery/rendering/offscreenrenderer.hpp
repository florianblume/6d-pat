#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include "model/objectmodel.hpp"

#include <QSize>
#include <QList>
#include <QImage>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOffscreenSurface>
#include <QMatrix4x4>
#include <QAtomicInt>
#include <QThread>

/*!
 * \brief The OffscreenRenderer class is a runnable that renders a given object model
 * offscreen and returns the image. This way preview renderings of object models can
 * be created.
 */
class OffscreenRenderer : public QObject {

    Q_OBJECT

public:
    OffscreenRenderer(const QList<ObjectModel> &objectModels, const QSize &size);
    void render();
    void stop();

Q_SIGNALS:
    void imageRendered(const QString &objectModel, int index, const QImage &renderedImage);
    void renderingFinished();

private:
    QList<ObjectModel> objectModels;
    QSize size;
    QOpenGLContext *context;
    QSurfaceFormat surfaceFormat;
    QOffscreenSurface *surface;
    QOpenGLShaderProgram *objectsProgram;
    QOpenGLFramebufferObject *renderFbo;
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 modelMatrix;
    float nearPlane = 5.0;
    float farPlane = 400.0;
    QAtomicInt running = 1;
    void shutdown();
};

#endif // OFFSCREENRENDERER_H
