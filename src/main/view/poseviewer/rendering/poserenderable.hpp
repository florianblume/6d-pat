#ifndef OBJECTMODELRENDERABLE_H
#define OBJECTMODELRENDERABLE_H

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
class PoseRenderable
{
public:
    PoseRenderable(const Pose &pose,
                          int vertexAttributeLoc,
                          int normalAttributeLoc);
    QOpenGLVertexArrayObject *getVertexArrayObject();
    QString getPoseId();
    int getIndicesCount();
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
    ObjectModel objectModel;
    QVector3D position;
    QMatrix3x3 rotation;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertexBuffer;
    QOpenGLBuffer normalBuffer;
    QOpenGLBuffer indexBuffer;
    QVector<GLfloat> vertices;
    QVector<GLfloat> normals;
    QVector<GLuint> indices;
    int vertexAttributeLoc = 0;
    int normalAttributeLoc = 0;
    QMatrix4x4 viewModelMatrix;

    void computeModelViewMatrix();
    void processMesh(aiMesh *mesh);
    void populateVertexArrayObject();
};

#endif // OBJECTMODELRENDERABLE_H
