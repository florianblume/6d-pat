#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, *pose.objectModel()),
        pose(pose),
        m_picker(new Qt3DRender::QObjectPicker),
        transform(new Qt3DCore::QTransform) {
    transform->setRotation(pose.rotation());
    transform->setTranslation(pose.position());
    addComponent(transform);
    addComponent(m_picker);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked, [this](Qt3DRender::QPickEvent *e){
        if (e->button() == Qt3DRender::QPickEvent::RightButton)
            this->setSelected(this->isSelected() ? false : true);
    });
}

ObjectModel PoseRenderable::objectModel() {
    return *pose.objectModel();
}

QVector3D PoseRenderable::position() {
    return pose.position();
}

void PoseRenderable::setPosition(const QVector3D &position) {
    pose.setPosition(position);
    transform->setTranslation(position);
}

QQuaternion PoseRenderable::rotation() {
    return pose.rotation();
}

void PoseRenderable::setRotation(const QQuaternion &rotation) {
    pose.setRotation(rotation);
    transform->setRotation(rotation);
}

QString PoseRenderable::poseID() {
    return pose.id();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return pose.id() == other.pose.id();
}
