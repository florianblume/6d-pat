#ifndef DATATEXTUREIMAGE_P_H
#define DATATEXTUREIMAGE_P_H

#include "datatextureimage.h"

#include <Qt3DRender/private/qabstracttextureimage_p.h>
#include <Qt3DRender/QTextureImageDataGenerator>
#include <Qt3DRender/private/qt3drender_global_p.h>

class Q_3DRENDERSHARED_PRIVATE_EXPORT DataTextureImagePrivate : public Qt3DRender::QAbstractTextureImagePrivate {

    Q_OBJECT

public:
    DataTextureImagePrivate();
    ~DataTextureImagePrivate();

    Q_DECLARE_PUBLIC(DataTextureImage)

    QVector<QVector3D> m_data;
    Qt3DRender::QTextureImageDataGeneratorPtr m_currentGenerator;

    // gets increased each time the image is re-painted.
    // used to distinguish between different generators
    quint64 m_generation;

    void updateData();
};

class DataTextureImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator {

public:
    DataTextureImageDataGenerator(const QVector<QVector3D> &data, int gen, Qt3DCore::QNodeId texId);
    ~DataTextureImageDataGenerator();

    // Will be executed from within a QAspectJob
    Qt3DRender::QTextureImageDataPtr operator ()() final;
    bool operator ==(const QTextureImageDataGenerator &other) const final;

    QT3D_FUNCTOR(DataTextureImageDataGenerator)

private:
    QVector<QVector3D> m_data;
    quint64 m_generation;
    Qt3DCore::QNodeId m_dataTextureImageId;
};

#endif // DATATEXTUREIMAGE_P_H
