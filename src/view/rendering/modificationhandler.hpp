#ifndef MODIFICATIONHANDLER_H
#define MODIFICATIONHANDLER_H

#include <QObject>
#include <Qt3DCore/QTransform>

class ModificationHandler : public QObject {

    Q_OBJECT

public:
    explicit ModificationHandler(QObject *parent = nullptr);

public Q_SLOTS:
    void setViewMatrix(const QMatrix4x4 &viewMatrix);
    void setProjectionMatrix(const QMatrix4x4 &projectionMatrix);
    void setTransform(Qt3DCore::QTransform *transform);
    void setSize(const QSize &size);
    void setHeight(int height);
    void setWidth(int width);

protected:
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QSize m_size;
    Qt3DCore::QTransform *m_transform;
};

#endif // MODIFICATIONHANDLER_H
