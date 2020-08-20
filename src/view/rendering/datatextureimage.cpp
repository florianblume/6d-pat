#include "datatextureimage.h"
#include "datatextureimage_p.h"

#include <QByteArray>
#include <Qt3DRender/QTextureImageData>

DataTextureImage::DataTextureImage(Qt3DCore::QNode *parent)
    : Qt3DRender::QAbstractTextureImage(parent) {
    setLayer(0);
    setMipLevel(0);
}

DataTextureImage::~DataTextureImage() {

}

QVector<QVector3D> DataTextureImage::data() const {
    Q_D(const DataTextureImage);
    return d->m_data;
}

void DataTextureImage::update(const QRect &rect) {
    Q_UNUSED(rect);
    Q_D(DataTextureImage);

    d->updateData();
}

void DataTextureImage::setData(QVector<QVector3D> newData) {
    Q_D(DataTextureImage);
    if (d->m_data != newData) {
        d->m_data = newData;
        Q_EMIT dataChanged(newData);
        d->updateData();
    }
}

Qt3DRender::QTextureImageDataGeneratorPtr DataTextureImage::dataGenerator() const {
    Q_D(const DataTextureImage);
    return d->m_currentGenerator;
}

void DataTextureImagePrivate::updateData() {
    ++m_generation;
    m_currentGenerator = QSharedPointer<DataTextureImageDataGenerator>::create(m_data, m_generation, q_func()->id());
    q_func()->notifyDataGeneratorChanged();
}

DataTextureImageDataGenerator::DataTextureImageDataGenerator(const QVector<QVector3D> &data, int gen, Qt3DCore::QNodeId texId)
    : m_data(data)
    , m_generation(gen)
    , m_dataTextureImageId(texId) {

}

DataTextureImageDataGenerator::~DataTextureImageDataGenerator() {

}

Qt3DRender::QTextureImageDataPtr DataTextureImageDataGenerator::operator ()() {
    Qt3DRender::QTextureImageDataPtr textureData = Qt3DRender::QTextureImageDataPtr::create();

    // Convert data to float array
    float floatData[m_data.count() * 3];
    for (int i = 0; i < m_data.count(); i +=3) {
        floatData[i] = m_data[i].x();
        floatData[i + 1] = m_data[i].y();
        floatData[i + 2] = m_data[i].z();
    }

    QByteArray array;
    int len = sizeof(floatData);
    memcpy(array.data(), &floatData, len);
    textureData->setData(array, 4);

    textureData->setFormat(QOpenGLTexture::RGB32F);
    textureData->setPixelType(QOpenGLTexture::Float32);
    textureData->setPixelFormat(QOpenGLTexture::RGB);
    return textureData;
}

bool DataTextureImageDataGenerator::operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const {
    const DataTextureImageDataGenerator *otherFunctor = functor_cast<DataTextureImageDataGenerator>(&other);
        return (otherFunctor != nullptr && otherFunctor->m_generation == m_generation && otherFunctor->m_dataTextureImageId == m_dataTextureImageId);
}
