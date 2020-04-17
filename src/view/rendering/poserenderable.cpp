#include "poserenderable.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

PoseRenderable::PoseRenderable(const Pose &pose,
                               int vertexAttributeLoc,
                               int normalAttributeLoc) :
    ObjectModelRenderable(*pose.getObjectModel(), vertexAttributeLoc, normalAttributeLoc),
    poseId(pose.getID()),
    position(pose.getPosition()),
    rotation(pose.getRotation()) {

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
        this->processMesh(scene->mMeshes[0]);
    }
    populateVertexArrayObject();
}

QMatrix4x4 PoseRenderable::getModelViewMatrix() {
    return modelViewMatrix;
}

ObjectModel PoseRenderable::getObjectModel() {
    return objectModel;
}

QVector3D PoseRenderable::getPosition() {
    return position;
}

void PoseRenderable::setPosition(QVector3D position) {
    this->position = position;
    computeModelViewMatrix();
}

QMatrix3x3 PoseRenderable::getRotation() {
    return rotation;
}

void PoseRenderable::setRotation(QMatrix3x3 rotation) {
    this->rotation = rotation;
    computeModelViewMatrix();
}

QString PoseRenderable::getPoseId() {
    return poseId;
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return poseId == other.poseId;
}

// Private functions from here

void PoseRenderable::computeModelViewMatrix() {
    modelViewMatrix = QMatrix4x4(rotation);
    modelViewMatrix(0, 3) = position[0];
    modelViewMatrix(1, 3) = position[1];
    modelViewMatrix(2, 3) = position[2];
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    modelViewMatrix = yz_flip * modelViewMatrix;
}
