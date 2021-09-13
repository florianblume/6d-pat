#include "texturerendertarget.hpp"

OffscreenTextureRenderTarget::OffscreenTextureRenderTarget(
        Qt3DCore::QNode *parent,
        const QSize &size,
        Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint,
        bool withMultisampling)
    : Qt3DRender::QRenderTarget(parent)
    , m_size(size) {

    // Create a render target output for rendering colour.
    m_colorOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_colorOutput->setAttachmentPoint(attatchmentPoint);
    // Create a texture to render into.
    if (withMultisampling) {
        m_colorTexture = new Qt3DRender::QTexture2DMultisample(m_colorOutput);
    } else {
        m_colorTexture = new Qt3DRender::QTexture2D(m_colorOutput);
    }
    m_colorTexture->setSize(size.width(), size.height());
    m_colorTexture->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    m_colorTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_colorTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    // Hook the texture up to our output, and the output up to this object.
    m_colorOutput->setTexture(m_colorTexture);
    addOutput(m_colorOutput);

    // Create a render target output for depth
    m_depthTextureOutput = new Qt3DRender::QRenderTargetOutput(this);
    m_depthTextureOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);

    if (withMultisampling) {
        m_depthTexture = new Qt3DRender::QTexture2DMultisample(m_depthTextureOutput);
    } else {
        m_depthTexture = new Qt3DRender::QTexture2D(m_depthTextureOutput);
    }
    m_depthTexture->setSize(size.width(), size.height());
    m_depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    m_depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    m_depthTexture->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
    m_depthTexture->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
    // Hook up the depth texture
    m_depthTextureOutput->setTexture(m_depthTexture);
    addOutput(m_depthTextureOutput);
}

Qt3DRender::QAbstractTexture* OffscreenTextureRenderTarget::colorTexture() {
    return m_colorTexture;
}

Qt3DRender::QAbstractTexture *OffscreenTextureRenderTarget::depthTexture() {
    return m_depthTexture;
}

void OffscreenTextureRenderTarget::setSize(const QSize &size) {
    m_size = size;
    m_colorTexture->setSize(size.width(), size.height());
    m_depthTexture->setSize(size.width(), size.height());
}

QSize OffscreenTextureRenderTarget::size() {
    return m_size;
}

void OffscreenTextureRenderTarget::setSamples(int samples) {
    m_colorTexture->setSamples(samples);
    m_depthTexture->setSamples(samples);
}
