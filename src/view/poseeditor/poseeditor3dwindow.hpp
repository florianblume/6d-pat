#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"

#include <QString>
#include <QVector>
#include <QVector3D>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QObjectPicker>

class PoseEditor3DWindow : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT

public:
    explicit PoseEditor3DWindow();
    void setObjectModel(const ObjectModel &objectModel);
    void setRotationOfObjectModel(QVector3D rotation);
    void addClick(QVector3D position, QColor color);
    void removeClicks();
    void reset();

    ~PoseEditor3DWindow();

Q_SIGNALS:
    void positionClicked(QVector3D position);

private Q_SLOTS:
    void onSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    QPoint lastClicked2DPos;
    QVector<QVector3D> clicks3D;
    QVector<QVector3D> clickColors;
    Qt3DCore::QEntity *rootEntity;
    QVector<Qt3DCore::QEntity*> clickSpheres;
    Qt3DRender::QSceneLoader *sceneLoader;
    Qt3DRender::QObjectPicker *picker;

    Qt3DCore::QEntity* createClickSphere(QVector3D position);
};

#endif
