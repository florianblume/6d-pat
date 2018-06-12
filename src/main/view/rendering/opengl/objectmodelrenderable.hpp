#ifndef OBJECTMODELRENDERABLE_H
#define OBJECTMODELRENDERABLE_H

#include "model/correspondence.hpp"

#include <assimp/mesh.h>
#include <assimp/Importer.hpp>

#include <QVector>
#include <QVector3D>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix3x3>
#include <QMatrix4x4>

class ObjectModelRenderable
{
public:
    ObjectModelRenderable(const Correspondence &correspondence,
                          int vertexAttributeLoc,
                          int normalAttributeLoc);
    QOpenGLVertexArrayObject *getVertexArrayObject();
    QString getCorrespondenceId();
    int getIndicesCount();
    QMatrix4x4 getViewMatrix();
    ObjectModel getObjectModel();
    QVector3D getPosition();
    void setPosition(QVector3D position);
    QMatrix3x3 getRotation();
    void setRotation(QMatrix3x3 rotation);

    // To retrieve the respective renderable
    bool operator==(const ObjectModelRenderable &other);

private:
    QString correspondenceId;
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
    QMatrix4x4 viewMatrix;

    void computeViewMatrix();
    void processMesh(aiMesh *mesh);
    void populateVertexArrayObject();
};

#endif // OBJECTMODELRENDERABLE_H
