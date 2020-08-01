#include "texturerendertarget.h"

TextureRenderTarget::TextureRenderTarget(Qt3DCore::QNode *parent, 
                                         const QSize &size,
                                         Qt3DRender::QRenderTargetOutput::AttachmentPoint attatchmentPoint) :
    Qt3DRender::QRenderTarget(parent),
    size(size)
{
    // The lifetime of the objects created here is managed
    // automatically, as they become children of this object.

    // Create a render target output for rendering colour.
    output = new Qt3DRender::QRenderTargetOutput(this);
    output->setAttachmentPoint(attatchmentPoint);

    // Create a texture to render into.
    texture = new Qt3DRender::QTexture2D(output);
    texture->setSize(size.width(), size.height());
    texture->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    texture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

    // Hook the texture up to our output, and the output up to this object.
    output->setTexture(texture);
    addOutput(output);
        
    depthTextureOutput = new Qt3DRender::QRenderTargetOutput(this);
    depthTextureOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);
    depthTexture = new Qt3DRender::QTexture2D(depthTextureOutput);
    depthTexture->setSize(size.width(), size.height());
    depthTexture->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    depthTexture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
    depthTexture->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);
    // Hook up the depth texture
    depthTextureOutput->setTexture(depthTexture);
    addOutput(depthTextureOutput);
}

Qt3DRender::QTexture2D* TextureRenderTarget::getTexture()
{
    return texture;
}

void TextureRenderTarget::setSize(const QSize &size)
{
    this->size = size;
    texture->setSize(size.width(), size.height());
}
