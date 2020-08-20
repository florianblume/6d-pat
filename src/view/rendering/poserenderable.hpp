#ifndef POSERENDERABLE_H
#define POSERENDERABLE_H

#include "objectrenderable2.hpp"
#include "model/pose.hpp"

#include <assimp/mesh.h>
#include <assimp/Importer.hpp>

#include <QVector>
#include <QVector3D>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix3x3>
#include <QMatrix4x4>

//!
//! \brief The PoseRenderable class is only an object model renderable
//! essentially (i.e. displays an object model) but takes in a pose
//! to compute the position of the object according to the pose.
//!
class PoseRenderable : public ObjectModelRenderable
{
public:
    PoseRenderable(const Pose &pose,
                          int vertexAttributeLoc,
                          int normalAttributeLoc);
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
    QString poseId;
    QVector3D position;
    QMatrix3x3 rotation;
    QMatrix4x4 modelViewMatrix;

    void computeModelViewMatrix();
};

#endif // POSERENDERABLE_H
