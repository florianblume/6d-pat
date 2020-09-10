#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
    ObjectModelRenderable(parent, pose.getObjectModel()),
    pose(pose) {
}

QMatrix4x4 PoseRenderable::getModelViewMatrix() {
    return modelViewMatrix;
}

ObjectModel PoseRenderable::getObjectModel() {
    return *pose.getObjectModel();
}

QVector3D PoseRenderable::getPosition() {
    return pose.getPosition();
}

void PoseRenderable::setPosition(QVector3D position) {
    pose.setPosition(position);
}

QMatrix3x3 PoseRenderable::getRotation() {
    return pose.getRotation();
}

void PoseRenderable::setRotation(QMatrix3x3 rotation) {
    pose.setRotation(rotation);
}

QString PoseRenderable::getPoseId() {
    return pose.getID();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.getID() == other.pose.getID();
}

void PoseRenderable::computeModelViewMatrix() {
    /*
    modelViewMatrix = QMatrix4x4(rotation);
    modelViewMatrix(0, 3) = position[0];
    modelViewMatrix(1, 3) = position[1];
    modelViewMatrix(2, 3) = position[2];
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    modelViewMatrix = yz_flip * modelViewMatrix;
    */
}
