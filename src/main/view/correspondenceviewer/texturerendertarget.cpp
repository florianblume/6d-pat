#include "texturerendertarget.h"

TextureRenderTarget::TextureRenderTarget(Qt3DCore::QNode *parent, const QSize &size) :
    Qt3DRender::QRenderTarget(parent),
    size(size)
{
    output = new Qt3DRender::QRenderTargetOutput(this);                   // no need to manage memory, we lose possession
    output->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
    texture = new Qt3DRender::QTexture2D(output);                         // no need to manage memory, we lose possession
    texture->setSize(size.width(), size.height());
    texture->setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
    texture->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    texture->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    output->setTexture(texture);
    addOutput(output);
}

Qt3DRender::QTexture2D* TextureRenderTarget::getTexture() {
    return texture;
}

void TextureRenderTarget::setSize(const QSize &size) {
    this->size = size;
    texture->setSize(size.width(), size.height());
}
