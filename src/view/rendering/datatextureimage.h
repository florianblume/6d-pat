#ifndef TESTPAINTEDTEXTUREIMAGE_H
#define TESTPAINTEDTEXTUREIMAGE_H

#include <QVector>
#include <QVector3D>
#include <Qt3DRender/qabstracttextureimage.h>

class TestPaintedTextureImagePrivate;

class Q_3DRENDERSHARED_EXPORT TestPaintedTextureImage : public Qt3DRender::QAbstractTextureImage
{
    Q_OBJECT
    Q_PROPERTY(QVector<QVector3D> data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit TestPaintedTextureImage(Qt3DCore::QNode *parent = nullptr);
    ~TestPaintedTextureImage();

    QVector<QVector3D> data();
    int width();
    int height();

    void update(const QRect &rect = QRect());

public Q_SLOTS:
    void setData(QVector<QVector3D> newData);

Q_SIGNALS:
    void dataChanged(QVector<QVector3D> data);

private:
    Q_DECLARE_PRIVATE(TestPaintedTextureImage)

    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
};

#endif
