#ifndef POSERENDERABLE_H
#define POSERENDERABLE_H

#include "objectmodelrenderable.hpp"
#include "model/pose.hpp"


#include <QVector>
#include <QVector3D>
#include <QMatrix3x3>
#include <QMatrix4x4>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

//!
//! \brief The PoseRenderable class is only an object model renderable
//! essentially (i.e. displays an object model) but takes in a pose
//! to compute the position of the object according to the pose.
//!
class PoseRenderable : public ObjectModelRenderable
{
public:
    PoseRenderable(Qt3DCore::QEntity *parent,
                   const Pose &pose);
    QString getPoseId();
    QMatrix4x4 getModelViewMatrix();
    ObjectModel getObjectModel();
    QVector3D getPosition();
    void setPosition(QVector3D position);
    QMatrix3x3 getRotation();
    void setRotation(QMatrix3x3 rotation);

    // To retrieve the respective renderable
    bool operator==(const PoseRenderable &other);

private:
    Pose pose;

    QMatrix4x4 modelViewMatrix;

    Qt3DCore::QTransform *transform;

    void computeModelViewMatrix();
};

#endif // POSERENDERABLE_H
