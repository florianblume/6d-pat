#ifndef OBJECTMODELRENDERABLE_H
#define OBJECTMODELRENDERABLE_H

#include "model/objectmodel.hpp"

#include <assimp/mesh.h>
#include <assimp/Importer.hpp>

#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class ObjectModelRenderable
{
public:
    ObjectModelRenderable(const ObjectModel &objectModel,
                          int vertexAttributeLoc,
                          int normalAttributeLoc);
    QOpenGLVertexArrayObject *getVertexArrayObject();
    QMatrix4x4 getModelMatrix();
    int getIndicesCount();
    ObjectModel getObjectModel();
    QVector3D getPosition();
    void setPosition(QVector3D position);
    QMatrix3x3 getRotation();
    void setRotation(QMatrix3x3 rotation);

private:
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
    QMatrix4x4 modelMatrix;

    void computeModelMatrix();
    void processMesh(aiMesh *mesh);
    void populateVertexArrayObject();
};

#endif // OBJECTMODELRENDERABLE_H
