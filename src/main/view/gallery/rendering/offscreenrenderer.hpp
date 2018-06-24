#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include "model/objectmodel.hpp"

#include <QRunnable>
#include <QSize>
#include <QImage>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOffscreenSurface>
#include <QMatrix4x4>
#include <QMutex>

class OffscreenRenderer : public QObject, public QRunnable {

    Q_OBJECT

public:
    OffscreenRenderer(const ObjectModel &objectModel, const QSize &size);
    void run() override;
    QImage getImage();
    ObjectModel getObjectModel();

signals:
    void imageReady();

private:
    QMutex mutex;
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
    float nearPlane = 1.0;
    float farPlane = 500.0;

    void setupProgram();
};

#endif // OFFSCREENRENDERER_H
