#include "datatextureimage.h"
#include "datatextureimage_p.h"

/*!
    \class Qt3DRender::QPaintedTextureImage
    \inmodule Qt3DRender
    \since 5.8
    \brief A QAbstractTextureImage that can be written through a QPainter.
    A QPaintedTextureImage provides a way to specify a texture image
    (and thus an OpenGL texture) through a QPainter. The width and height of the
    texture image can be specified through the width and height or size
    properties.
    A QPaintedTextureImage must be subclassed and the virtual paint() function
    implemented. Each time update() is called on the QPaintedTextureImage,
    the paint() function is invoked and the resulting image is uploaded.
    The QPaintedTextureImage must be attached to some QAbstractTexture.
 */



TestPaintedTextureImagePrivate::TestPaintedTextureImagePrivate()
     : m_generation(0)
{
}

TestPaintedTextureImagePrivate::~TestPaintedTextureImagePrivate()
{
}

void TestPaintedTextureImagePrivate::updateData()
{
    ++m_generation;
    m_currentGenerator = QSharedPointer<PaintedTextureImageDataGenerator>::create(m_data, m_generation, q_func()->id());
    q_func()->notifyDataGeneratorChanged();
}

TestPaintedTextureImage::TestPaintedTextureImage(Qt3DCore::QNode *parent)
    : QAbstractTextureImage(*new TestPaintedTextureImagePrivate, parent)
{
}

TestPaintedTextureImage::~TestPaintedTextureImage()
{
}

QVector<QVector3D> TestPaintedTextureImage::data() {
    Q_D(TestPaintedTextureImage);
    return d->m_data;
}

int TestPaintedTextureImage::width()
{
    Q_D(TestPaintedTextureImage);
    return d->m_data.count();
}

int TestPaintedTextureImage::height()
{
    return 1;
}

/*!
   Schedules the painted texture's paint() function to be called,
   which in turn uploads the new image to the GPU.
   Parameter \a rect is currently unused.
*/
void TestPaintedTextureImage::update(const QRect &rect)
{
    Q_UNUSED(rect);
    Q_D(TestPaintedTextureImage);

    d->updateData();
}

void TestPaintedTextureImage::setData(QVector<QVector3D> newData) {
    Q_D(TestPaintedTextureImage);
    if (d->m_data != newData) {
        d->m_data = newData;
        d->updateData();
    }
}

/*!
    \fn Qt3DRender::QPaintedTextureImage::paint(QPainter *painter)
    Paints the texture image with the specified QPainter object \a painter.
*/
Qt3DRender::QTextureImageDataGeneratorPtr TestPaintedTextureImage::dataGenerator() const
{
    Q_D(const TestPaintedTextureImage);
    Qt3DRender::QTextureImageDataGeneratorPtr generator = d->m_currentGenerator;
    if (generator.isNull()) {
        generator = QSharedPointer<PaintedTextureImageDataGenerator>::create(d->m_data, d->m_generation, id());
    }
    return generator;
}


PaintedTextureImageDataGenerator::PaintedTextureImageDataGenerator(const QVector<QVector3D> &data, int gen, Qt3DCore::QNodeId texId)
    : m_data(data)    // pixels are implicitly shared, no copying
    , m_generation(gen)
    , m_paintedTextureImageId(texId)
{
}

PaintedTextureImageDataGenerator::~PaintedTextureImageDataGenerator()
{
}

Qt3DRender::QTextureImageDataPtr PaintedTextureImageDataGenerator::operator ()()
{
    Qt3DRender::QTextureImageDataPtr textureData = Qt3DRender::QTextureImageDataPtr::create();

    // Convert data to float array
    float floatData[m_data.count() * 3];
    for (int i = 0; i < m_data.count() * 3; i +=3) {
        floatData[i] = m_data[i / 3].x();
        floatData[i + 1] = m_data[i / 3].y();
        floatData[i + 2] = m_data[i / 3].z();
    }

    QByteArray array;
    int len = sizeof(floatData);
    memcpy(array.data(), &floatData, len);
    textureData->setData(array, 4);

    textureData->setFormat(QOpenGLTexture::RGB32F);
    textureData->setPixelType(QOpenGLTexture::Float32);
    textureData->setPixelFormat(QOpenGLTexture::RGB);

    textureData->setWidth(m_data.count());
    textureData->setHeight(1);
    textureData->setDepth(1);
    textureData->setFaces(1);
    textureData->setMipLevels(1);
    textureData->setLayers(1);
    textureData->setTarget(QOpenGLTexture::Target1D);
    return textureData;
}

bool PaintedTextureImageDataGenerator::operator ==(const Qt3DRender::QTextureImageDataGenerator &other) const
{
    const PaintedTextureImageDataGenerator *otherFunctor = functor_cast<PaintedTextureImageDataGenerator>(&other);
    return (otherFunctor != nullptr && otherFunctor->m_generation == m_generation && otherFunctor->m_paintedTextureImageId == m_paintedTextureImageId);
}
