#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, pose.getObjectModel()),
        pose(pose),
        picker(new Qt3DRender::QObjectPicker),
        transform(new Qt3DCore::QTransform) {
    transform->setRotation(QQuaternion::fromRotationMatrix(pose.getRotation()));
    transform->setTranslation(pose.getPosition());
    addComponent(transform);
    addComponent(picker);
    picker->setHoverEnabled(true);
    picker->setDragEnabled(true);
    connect(picker, &Qt3DRender::QObjectPicker::clicked, [this](Qt3DRender::QPickEvent *e){
        if (e->button() == Qt3DRender::QPickEvent::RightButton)
            this->setSelected(this->isSelected() ? false : true);
    });
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

QString PoseRenderable::getPoseId() {
    return pose.getID();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.getID() == other.pose.getID();
}
