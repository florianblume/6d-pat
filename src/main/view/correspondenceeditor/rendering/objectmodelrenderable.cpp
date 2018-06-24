#include "view/correspondenceeditor/rendering/objectmodelrenderable.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QtGlobal>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

ObjectModelRenderable::ObjectModelRenderable(const ObjectModel &objectModel,
                                             int vertexAttributeLoc,
                                             int normalAttributeLoc) :
    objectModel(objectModel),
    vertexBuffer(QOpenGLBuffer::VertexBuffer),
    normalBuffer(QOpenGLBuffer::VertexBuffer),
    indexBuffer(QOpenGLBuffer::IndexBuffer),
    vertexAttributeLoc(vertexAttributeLoc),
    normalAttributeLoc(normalAttributeLoc) {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objectModel.getAbsolutePath().toStdString(),
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType
                                             );
    for (uint i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[0]);
    }
    populateVertexArrayObject();
}

QOpenGLVertexArrayObject *ObjectModelRenderable::getVertexArrayObject() {
    return &vao;
}

int ObjectModelRenderable::getIndicesCount() {
    return indices.size();
}

ObjectModel ObjectModelRenderable::getObjectModel() {
    return objectModel;
}

float ObjectModelRenderable::getLargestVertexValue() {
    return largestVertexValue;
}

// Private functions from here

void ObjectModelRenderable::processMesh(aiMesh *mesh) {
    // Get Vertices
    if (mesh->mNumVertices > 0)
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mVertices[ii];

            vertices.push_back(vec.x);
            vertices.push_back(vec.y);
            vertices.push_back(vec.z);

            float m = qMax(vec.x, vec.y);
            m = qMax(m, vec.z);
            if (m > largestVertexValue)
                largestVertexValue = m;
        }
    }

    // Get Normals
    if (mesh->HasNormals())
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mNormals[ii];
            normals.push_back(vec.x);
            normals.push_back(vec.y);
            normals.push_back(vec.z);
        };
    }

    // Get mesh indexes
    for (uint t = 0; t < mesh->mNumFaces; ++t)
    {
        aiFace* face = &mesh->mFaces[t];
        if (face->mNumIndices != 3)
        {
            continue;
        }

        indices.push_back(face->mIndices[0]);
        indices.push_back(face->mIndices[1]);
        indices.push_back(face->mIndices[2]);
    }
}

void ObjectModelRenderable::populateVertexArrayObject() {
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    QOpenGLContext *context = QOpenGLContext::currentContext();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Setup the vertex buffer object.
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexBuffer.allocate(vertices.constData(),
                                    vertices.size() * sizeof(GLfloat));
    vertexBuffer.bind();
    f->glEnableVertexAttribArray(vertexAttributeLoc);
    f->glVertexAttribPointer(vertexAttributeLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Setup the normal buffer object.
    normalBuffer.create();
    normalBuffer.bind();
    normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    normalBuffer.allocate(normals.constData(),
                                    normals.size() * sizeof(GLfloat));
    normalBuffer.bind();
    f->glEnableVertexAttribArray(normalAttributeLoc);
    f->glVertexAttribPointer(normalAttributeLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Setup the index buffer object.
    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer.allocate(indices.constData(),
                                   indices.size() * sizeof(GLint));
}

