#include "correspondencerenderable.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

CorrespondenceRenderable::CorrespondenceRenderable(const Correspondence &correspondence,
                                             int vertexAttributeLoc,
                                             int normalAttributeLoc) :
    correspondenceId(correspondence.getID()),
    objectModel(*correspondence.getObjectModel()),
    position(correspondence.getPosition()),
    rotation(correspondence.getRotation()),
    vertexBuffer(QOpenGLBuffer::VertexBuffer),
    normalBuffer(QOpenGLBuffer::VertexBuffer),
    indexBuffer(QOpenGLBuffer::IndexBuffer),
    vertexAttributeLoc(vertexAttributeLoc),
    normalAttributeLoc(normalAttributeLoc) {

    computeModelViewMatrix();
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

QMatrix4x4 CorrespondenceRenderable::getModelViewMatrix() {
    return viewModelMatrix;
}

ObjectModel CorrespondenceRenderable::getObjectModel() {
    return objectModel;
}

QVector3D CorrespondenceRenderable::getPosition() {
    return position;
}

void CorrespondenceRenderable::setPosition(QVector3D position) {
    this->position = position;
    computeModelViewMatrix();
}

QMatrix3x3 CorrespondenceRenderable::getRotation() {
    return rotation;
}

void CorrespondenceRenderable::setRotation(QMatrix3x3 rotation) {
    this->rotation = rotation;
    computeModelViewMatrix();
}

QOpenGLVertexArrayObject *CorrespondenceRenderable::getVertexArrayObject() {
    return &vao;
}

QString CorrespondenceRenderable::getCorrespondenceId() {
    return correspondenceId;
}

int CorrespondenceRenderable::getIndicesCount() {
    return indices.size();
}

bool CorrespondenceRenderable::operator==(const CorrespondenceRenderable &other) {
    return correspondenceId == other.correspondenceId;
}

// Private functions from here

void CorrespondenceRenderable::computeModelViewMatrix() {
    viewModelMatrix = QMatrix4x4(rotation);
    viewModelMatrix(0, 3) = position[0];
    viewModelMatrix(1, 3) = position[1];
    viewModelMatrix(2, 3) = position[2];
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    viewModelMatrix = yz_flip * viewModelMatrix;
}

void CorrespondenceRenderable::processMesh(aiMesh *mesh) {
    // Get Vertices
    if (mesh->mNumVertices > 0)
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mVertices[ii];

            vertices.push_back(vec.x);
            vertices.push_back(vec.y);
            vertices.push_back(vec.z);
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

void CorrespondenceRenderable::populateVertexArrayObject() {
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
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
