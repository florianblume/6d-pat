#ifndef TEXTURERENDERTARGET_H
#define TEXTURERENDERTARGET_H

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

// Encapsulates a 2D texture that a frame graph can render offscreen into.
class OffscreenTextureRenderTarget : public Qt3DRender::QRenderTarget {

    Q_OBJECT

public:
    OffscreenTextureRenderTarget(Qt3DCore::QNode *parent = nullptr,
                                 const QSize &size = QSize(500, 500),
                                 Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint
                                                        = Qt3DRender::QRenderTargetOutput::Color0,
                                 bool withMultisampling = false);

    void setSize(const QSize &size);
    QSize size();
    void setSamples(int samples);
    Qt3DRender::QAbstractTexture* colorTexture();
    Qt3DRender::QAbstractTexture *depthTexture();

private:
    QSize m_size;
    Qt3DRender::QRenderTargetOutput *m_colorOutput;
    Qt3DRender::QAbstractTexture *m_colorTexture;
    // To enable depth testing
    Qt3DRender::QRenderTargetOutput *m_depthTextureOutput;
    Qt3DRender::QAbstractTexture *m_depthTexture;
};

#endif // TEXTURERENDERTARGET_H
