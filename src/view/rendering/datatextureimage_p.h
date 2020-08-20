#ifndef TESTPAINTEDTEXTUREIMAGEPRIVATE_H
#define TESTPAINTEDTEXTUREIMAGEPRIVATE_H

#include <Qt3DRender/private/qabstracttextureimage_p.h>
#include <Qt3DRender/qtextureimagedatagenerator.h>
#include "datatextureimage.h"
#include <Qt3DRender/private/qt3drender_global_p.h>

class Q_3DRENDERSHARED_PRIVATE_EXPORT TestPaintedTextureImagePrivate : public Qt3DRender::QAbstractTextureImagePrivate
{
public:
    TestPaintedTextureImagePrivate();
    ~TestPaintedTextureImagePrivate();

    Q_DECLARE_PUBLIC(TestPaintedTextureImage)

    QVector<QVector3D> m_data;
    Qt3DRender::QTextureImageDataGeneratorPtr m_currentGenerator;

    // gets increased each time the image is re-painted.
    // used to distinguish between different generators
    quint64 m_generation;

    void updateData();
};

class PaintedTextureImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
public:
    PaintedTextureImageDataGenerator(const QVector<QVector3D> &data, int gen, Qt3DCore::QNodeId texId);
    ~PaintedTextureImageDataGenerator();

    // Will be executed from within a QAspectJob
    Qt3DRender::QTextureImageDataPtr operator ()() final;
    bool operator ==(const QTextureImageDataGenerator &other) const final;

    QT3D_FUNCTOR(PaintedTextureImageDataGenerator)

private:
    QVector<QVector3D> m_data;
    quint64 m_generation;
    Qt3DCore::QNodeId m_paintedTextureImageId;
};

#endif
