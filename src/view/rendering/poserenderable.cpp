#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, pose.getObjectModel()),
        pose(pose),
        layerFilter(new Qt3DRender::QLayerFilter),
        layer(new Qt3DRender::QLayer),
        cameraSelector(new Qt3DRender::QCameraSelector),
        camera(new Qt3DRender::QCamera),
        picker(new Qt3DRender::QObjectPicker) {
    layerFilter->addLayer(layer);
    this->addComponent(layer);
    cameraSelector->setParent(layerFilter);
    cameraSelector->setCamera(camera);
    this->addComponent(picker);
    picker->setHoverEnabled(true);
    connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &PoseRenderable::clicked);
    connect(picker, &Qt3DRender::QObjectPicker::moved, this, &PoseRenderable::moved);
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

void PoseRenderable::setProjectionMatrix(QMatrix4x4 projectionMatrix) {
    this->camera->setProjectionMatrix(projectionMatrix);
}

Qt3DRender::QFrameGraphNode *PoseRenderable::frameGraph() {
    return layerFilter;
}

QString PoseRenderable::getPoseId() {
    return pose.getID();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.getID() == other.pose.getID();
}

void PoseRenderable::computeModelViewMatrix() {
    modelViewMatrix = QMatrix4x4(pose.getRotation());
    modelViewMatrix(0, 3) = pose.getPosition()[0];
    modelViewMatrix(1, 3) = pose.getPosition()[1];
    modelViewMatrix(2, 3) = pose.getPosition()[2];
    QMatrix4x4 yz_flip;
    yz_flip.setToIdentity();
    yz_flip(1, 1) = -1;
    yz_flip(2, 2) = -1;
    modelViewMatrix = yz_flip * modelViewMatrix;
}
