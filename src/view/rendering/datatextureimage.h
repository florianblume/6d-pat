#ifndef DATATEXTUREIMAGE_H
#define DATATEXTUREIMAGE_H

#include <QVector>
#include <QVector3D>

#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <Qt3DCore/QNode>

class DataTextureImagePrivate;

class Q_3DRENDERSHARED_EXPORT DataTextureImage : public Qt3DRender::QAbstractTextureImage
{
    Q_OBJECT
    Q_PROPERTY(QVector<QVector3D> data READ data WRITE setData)

public:
    DataTextureImage(Qt3DCore::QNode *parent = nullptr);
    ~DataTextureImage();

    QVector<QVector3D> data() const;
    void update(const QRect &rect = QRect());

public Q_SLOTS:
    void setData(QVector<QVector3D> newData);

Q_SIGNALS:
    void dataChanged(QVector<QVector3D> data);

private:
    Q_DECLARE_PRIVATE(DataTextureImage)
    Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
};

#endif // DATATEXTUREIMAGE_H
