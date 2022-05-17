#ifndef POSERENDERABLE_H
#define POSERENDERABLE_H

#include "objectmodelrenderable.hpp"
#include "model/pose.hpp"

#include <QObject>
#include <QList>
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
class PoseRenderable : public ObjectModelRenderable {
    Q_OBJECT

public:
    PoseRenderable(Qt3DCore::QEntity *parent, PosePtr pose);

    QString poseID();
    ObjectModelPtr objectModel();
    Qt3DCore::QTransform *transform() const;

    // To retrieve the respective renderable
    bool operator==(const PoseRenderable &other);

    PosePtr pose() const;

Q_SIGNALS:
    void clicked(Qt3DRender::QPickEvent *pickEvent);
    void moved(Qt3DRender::QPickEvent *pickEvent);
    void pressed(Qt3DRender::QPickEvent *pickEvent);
    void released(Qt3DRender::QPickEvent *pickEvent);
    void entered();
    void exited();

private:
    PosePtr m_pose;

    Qt3DRender::QObjectPicker *m_picker;
    Qt3DCore::QTransform *m_transform;
};

#endif // POSERENDERABLE_H
