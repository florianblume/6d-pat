#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"

#include <QString>
#include <QVector>
#include <QVector3D>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DExtras/QOrbitCameraController>

class PoseEditor3DWindow : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT

public:
    explicit PoseEditor3DWindow();
    void setObjectModel(const ObjectModel &objectModel);
    void addClick(QVector3D position);
    void removeClicks();
    void reset();

    ~PoseEditor3DWindow();

Q_SIGNALS:
    void positionClicked(QVector3D position);

private Q_SLOTS:
    void onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    QPoint lastClicked2DPos;
    QVector<QVector3D> clicks3D;
    QVector<QVector3D> clickColors;
    Qt3DCore::QEntity *rootEntity;
    Qt3DRender::QObjectPicker *picker;
    Qt3DExtras::QOrbitCameraController *cameraController;
    ObjectModelRenderable *objectModelRenderable;
};

#endif
