#include "texturerendertarget.h"

TextureRenderTarget::TextureRenderTarget(Qt3DCore::QNode *parent,
                                         const QSize &size,
                                         Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint) :
    Qt3DRender::QRenderTarget(parent),
    size(size)
{
    // Setup render target to render the final color into
    textureOutput = new Qt3DRender::QRenderTargetOutput(this);                   // no need to manage memory, we lose possession
    textureOutput->setAttachmentPoint(attatchmentPoint);
    texture = new Qt3DRender::QTexture2D(textureOutput);                         // no need to manage memory, we lose possession
    texture->setSize(size.width(), size.height());
    texture->setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    texture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    textureOutput->setTexture(texture);
    addOutput(textureOutput);

    depthTextureOutput = new Qt3DRender::QRenderTargetOutput(this);
    depthTextureOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    depthTexture = new Qt3DRender::QTexture2D(depthTextureOutput);
    depthTexture->setSize(size.width(), size.height());
    depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLess);
    depthTexture->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
    depthTextureOutput->setTexture(depthTexture);
    addOutput(depthTextureOutput);
}

void TextureRenderTarget::setSize(const QSize &size) {
    this->size = size;
    texture->setSize(size.width(), size.height());
    depthTexture->setSize(size.width(), size.height());
}
