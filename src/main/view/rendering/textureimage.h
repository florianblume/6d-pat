
#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H

#include <Qt3DRender/QPaintedTextureImage>
#include <QPainter>

class TextureImage : public Qt3DRender::QPaintedTextureImage {

public:
    TextureImage(Qt3DCore::QNode *parent = Q_NULLPTR);
    void paint(QPainter *painter) override;
    void setImagePath(const QString &imagePath);
    QString getImagePath();

private:
    QString imagePath;

};

#endif // IMAGETEXTURE_H
