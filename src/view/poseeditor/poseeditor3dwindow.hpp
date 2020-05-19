#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"

#include <QString>
#include <QTimer>
#include <QList>
#include <QSharedPointer>
#include <QVector>
#include <QVector3D>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>

typedef QSharedPointer<const ObjectModel> ObjectModelPtr;

class PoseEditor3DWindow : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT

public:
    explicit PoseEditor3DWindow();
    void setObjectModel(const ObjectModel *objectModel);
    void setRotationOfObjectModel(QVector3D rotation);
    void addClick(QVector3D position, QColor color);
    void removeClicks();
    void reset();

    ~PoseEditor3DWindow();

Q_SIGNALS:
    void positionClicked(QVector3D position);
    void rotationXChanged(float angle);
    void rotationYChanged(float angle);
    void rotationZChanged(float angle);

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    QPoint lastClicked2DPos;
    QVector<QVector3D> clicks3D;
    QVector<QVector3D> clickColors;
    ObjectModelPtr objectModel;
    Qt3DCore::QEntity *rootEntity;
    Qt3DRender::QSceneLoader *sceneLoader;
};

#endif
