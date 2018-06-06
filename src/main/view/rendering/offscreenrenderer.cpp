#include "offscreenrenderer.h"

#include <QImage>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QOpenGLContext>

OffscreenRenderer::OffscreenRenderer(const QSize &size) : size(size) {
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(3);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    //surfaceFormat.setOption(QSurfaceFormat::DeprecatedFunctions);
}

void OffscreenRenderer::setSize(const QSize &size) {
    this->size = size;
}

void OffscreenRenderer::setBackgroundImage(QImage image) {
    this->backgroundImage = image;
}

QImage OffscreenRenderer::render() {
    if (!renderFbo) {
        // Initialize context etc
        context = new QOpenGLContext();
        context->setFormat(surfaceFormat);
        context->create();
        surface = new QOffscreenSurface();
        surface->setFormat(context->format());
        surface->create();
        context->makeCurrent(surface);
        // Initialize the buffers and renderer
        QOpenGLFramebufferObjectFormat muliSampleFormat;
        muliSampleFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        muliSampleFormat.setMipmap(true);
        muliSampleFormat.setSamples(8);
        muliSampleFormat.setTextureTarget(GL_TEXTURE_2D);
        muliSampleFormat.setInternalTextureFormat(GL_RGBA32F_ARB);
        renderFbo = new QOpenGLFramebufferObject(size, muliSampleFormat);
        QOpenGLFramebufferObjectFormat downSampledFormat;
        downSampledFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        downSampledFormat.setMipmap(true);
        downSampledFormat.setTextureTarget(GL_TEXTURE_2D);
        downSampledFormat.setInternalTextureFormat(GL_RGBA32F_ARB);
        displayFbo = new QOpenGLFramebufferObject(size, downSampledFormat);
        logoRenderer = new LogoRenderer();
        logoRenderer->initialize();
    } else {
        context->makeCurrent(surface);
    }

    renderFbo->bind();
    context->functions()->glViewport(0, 0, size.width(), size.height());

    context->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    context->functions()->glClearColor(0.5f, 0.5f, 0.7f, 1.0f);

    QOpenGLPaintDevice device(size);
    QPainter painter;
    painter.begin(&device);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    QRect drawRect = QRect(0, 0, size.width(), size.height());

    painter.fillRect(drawRect, Qt::blue);

    //painter.drawPixmap(drawRect, QPixmap::fromImage(this->backgroundImage));

    painter.setPen(QPen(Qt::green, 5));
    painter.setBrush(Qt::red);
    painter.drawEllipse(0, 100, 400, 200);
    painter.drawEllipse(100, 0, 200, 400);

    painter.setPen(QPen(Qt::white, 0));
    QFont font;
    font.setPointSize(24);
    painter.setFont(font);
    painter.drawText(drawRect, "Hello FBO", QTextOption(Qt::AlignCenter));

    painter.end();

    // Clear depth buffer again because of background image
    context->functions()->glClear(GL_DEPTH_BUFFER_BIT);
    context->functions()->glEnable(GL_BLEND);
    context->functions()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    logoRenderer->render();
    context->functions()->glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // We need to flush the contents to the FBO before posting
    // the texture to the other thread, otherwise, we might
    // get unexpected results.
    context->functions()->glFlush();

    renderFbo->bindDefault();
    QOpenGLFramebufferObject::blitFramebuffer(displayFbo, renderFbo,
                                              GL_COLOR_BUFFER_BIT |
                                              GL_DEPTH_BUFFER_BIT |
                                              GL_STENCIL_BUFFER_BIT,GL_NEAREST);
    QImage image = displayFbo->toImage();
    context->doneCurrent();
    return image;
}

void OffscreenRenderer::shutdown() {
    context->makeCurrent(surface);
    delete renderFbo;
    delete displayFbo;
    delete logoRenderer;
    context->doneCurrent();
    delete context;
}
