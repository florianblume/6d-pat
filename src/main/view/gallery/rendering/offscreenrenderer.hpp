#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include "model/objectmodel.hpp"

#include <QSize>
#include <QImage>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOffscreenSurface>
#include <QMatrix4x4>
#include <QRunnable>

/*!
 * \brief The OffscreenRenderer class is a runnable that renders a given object model
 * offscreen and returns the image. This way preview renderings of object models can
 * be created.
 */
class OffscreenRenderer : public QObject, public QRunnable {

    Q_OBJECT

public:
    OffscreenRenderer(const ObjectModel &objectModel, const QSize &size);
    void run() override;
    QImage getImage();
    ObjectModel getObjectModel();

Q_SIGNALS:
    void imageReady();

private:
    ObjectModel objectModel;
    QImage image;
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

    void setupProgram();
};

#endif // OFFSCREENRENDERER_H
