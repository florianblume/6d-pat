#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, pose.getObjectModel()),
        pose(pose),
        layerFilter(new Qt3DRender::QLayerFilter),
        layer(new Qt3DRender::QLayer),
        cameraSelector(new Qt3DRender::QCameraSelector),
        camera(new Qt3DRender::QCamera),
        picker(new Qt3DRender::QObjectPicker),
        transform(new Qt3DCore::QTransform) {
    cameraSelector->setCamera(camera);
    addComponent(layer);
    layerFilter->addLayer(layer);
    transform->setRotation(QQuaternion::fromRotationMatrix(pose.getRotation()));
    transform->setTranslation(pose.getPosition());
    addComponent(transform);
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
    transform->setTranslation(position);
}

QMatrix3x3 PoseRenderable::getRotation() {
    return pose.getRotation();
}

void PoseRenderable::setRotation(QMatrix3x3 rotation) {
    pose.setRotation(rotation);
    transform->setRotation(QQuaternion::fromRotationMatrix(rotation));
}

void PoseRenderable::setProjectionMatrix(QMatrix4x4 projectionMatrix) {
    this->camera->setProjectionMatrix(projectionMatrix);
}

Qt3DRender::QFrameGraphNode *PoseRenderable::frameGraph() {
    return cameraSelector;
}

QString PoseRenderable::getPoseId() {
    return pose.getID();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.getID() == other.pose.getID();
}
