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
#include <Qt3DRender/QObjectPicker>

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

    QString poseID();
    ObjectModel objectModel();
    QVector3D position();
    void setPosition(const QVector3D &position);
    QQuaternion rotation();
    void setRotation(const QQuaternion &rotation);

    // To retrieve the respective renderable
    bool operator==(const PoseRenderable &other);

Q_SIGNALS:
    void clicked(Qt3DRender::QPickEvent *pickEvent);
    void moved(Qt3DRender::QPickEvent *pickEvent);

private:
    Pose pose;

    Qt3DRender::QObjectPicker *m_picker;
    Qt3DCore::QTransform *transform;
};

#endif // POSERENDERABLE_H
