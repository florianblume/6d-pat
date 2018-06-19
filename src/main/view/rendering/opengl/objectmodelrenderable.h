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
    int getIndicesCount();
    ObjectModel getObjectModel();
    float getLargestVertexValue();

private:
    ObjectModel objectModel;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertexBuffer;
    float largestVertexValue = 0.f;
    QOpenGLBuffer normalBuffer;
    QOpenGLBuffer indexBuffer;
    QVector<GLfloat> vertices;
    QVector<GLfloat> normals;
    QVector<GLuint> indices;
    int vertexAttributeLoc = 0;
    int normalAttributeLoc = 0;

    void processMesh(aiMesh *mesh);
    void populateVertexArrayObject();
};

#endif // OBJECTMODELRENDERABLE_H
