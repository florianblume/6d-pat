#ifndef BACKGROUNDIMAGERENDERABLE_H
#define BACKGROUNDIMAGERENDERABLE_H

#include "model/image.hpp"
#include "view/rendering/planerenderable.hpp"

#include <Qt3DRender/QTextureImage>

class BackgroundImageRenderable : public PlaneRenderable
{
public:
    BackgroundImageRenderable(Qt3DCore::QNode *parent,
                              const QString &image);
    void setImage(const QString &image);

private:
    Qt3DRender::QTextureImage *textureImage;
};

#endif // BACKGROUNDIMAGERENDERABLE_H
