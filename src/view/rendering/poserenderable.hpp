#ifndef POSERENDERABLE_H
#define POSERENDERABLE_H

#include "objectmodelrenderable.hpp"
#include "model/pose.hpp"

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QMatrix3x3>
#include <QMatrix4x4>

#include <Qt3DRender/QPickEvent>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QParameter>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

//!
//! \brief The PoseRenderable class is only an object model renderable
//! essentially (i.e. displays an object model) but takes in a pose
//! to compute the position of the object according to the pose.
//!
class PoseRenderable : public ObjectModelRenderable
{
    Q_OBJECT

public:
    PoseRenderable(Qt3DCore::QEntity *parent, const Pose &pose);

    QString getPoseId();
    QMatrix4x4 getModelViewMatrix();
    ObjectModel getObjectModel();
    QVector3D getPosition();
    void setPosition(QVector3D position);
    QMatrix3x3 getRotation();
    void setRotation(QMatrix3x3 rotation);
    void setProjectionMatrix(QMatrix4x4 projectionMatrix);
    Qt3DRender::QFrameGraphNode *frameGraph();

    // To retrieve the respective renderable
    bool operator==(const PoseRenderable &other);

Q_SIGNALS:
    void clicked(Qt3DRender::QPickEvent *pickEvent);
    void moved(Qt3DRender::QPickEvent *pickEvent);

private:
    Pose pose;

    Qt3DRender::QLayerFilter *layerFilter;
    Qt3DRender::QLayer *layer;
    Qt3DRender::QCameraSelector *cameraSelector;
    Qt3DRender::QCamera *camera;
    Qt3DRender::QObjectPicker *picker;

    QMatrix4x4 modelViewMatrix;

    Qt3DCore::QTransform *transform;
};

#endif // POSERENDERABLE_H
