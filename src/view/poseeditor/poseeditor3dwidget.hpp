#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "model/objectmodel.hpp"
#include "view/rendering/objectmodelrenderable.hpp"

#include <QString>
#include <QList>
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
    void setClicks(const QList<QVector3D> &clicks);
    void reset();

    void mousePressEvent(QMouseEvent *e) override;

    ~PoseEditor3DWindow();

Q_SIGNALS:
    void positionClicked(const QVector3D &position);
    void loadingObjectModel();
    void objectModelLoaded();

private Q_SLOTS:
    void onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void onPoseRenderableMoved();

private:
    Qt3DCore::QEntity *rootEntity;
    Qt3DRender::QObjectPicker *picker;
    Qt3DExtras::QOrbitCameraController *cameraController;
    ObjectModelRenderable *objectModelRenderable;

    bool mouseDown = false;
    bool mouseMoved = false;
};

#endif
