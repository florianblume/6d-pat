#ifndef OFFSCREENRENDERER_H
#define OFFSCREENRENDERER_H

#include "logorenderer.h"

#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

class OffscreenRenderer
{
public:
    OffscreenRenderer(const QSize &size);
    void setSize(const QSize &size);
    void setBackgroundImage(QImage image);
    QImage render();
    void shutdown();

private:
    QImage backgroundImage;
    QSize size;
    QOpenGLContext *context;
    QSurfaceFormat surfaceFormat;
    QOffscreenSurface *surface;
    QOpenGLFramebufferObject *renderFbo = 0;
    QOpenGLFramebufferObject *displayFbo;
    LogoRenderer *logoRenderer;
};

#endif // OFFSCREENRENDERER_H
