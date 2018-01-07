#ifndef TEXTURERENDERTARGET_H
#define TEXTURERENDERTARGET_H

#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QTexture>

class TextureRenderTarget : public Qt3DRender::QRenderTarget
{

public:
    TextureRenderTarget(Qt3DCore::QNode *parent = nullptr, const QSize &size = QSize(500, 500));
    Qt3DRender::QTexture2D *getTexture();
    void setSize(const QSize &size);
    QSize getSize() { return size; }

private:
    QSize size;
    Qt3DRender::QRenderTargetOutput *output;
    Qt3DRender::QTexture2D *texture;

};

#endif // TEXTURERENDERTARGET_H
