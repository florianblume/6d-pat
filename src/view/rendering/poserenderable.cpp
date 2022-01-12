#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               const Pose &pose) :
        ObjectModelRenderable(parent, pose.objectModel()),
        m_pose(pose),
        m_picker(new Qt3DRender::QObjectPicker),
        m_transform(new Qt3DCore::QTransform) {
    m_transform->setRotation(pose.rotation());
    m_transform->setTranslation(pose.position());
    addComponent(m_transform);
    addComponent(m_picker);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
            this, &PoseRenderable::clicked);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &PoseRenderable::moved);
    connect(m_picker, &Qt3DRender::QObjectPicker::pressed,
            this, &PoseRenderable::pressed);
    connect(m_picker, &Qt3DRender::QObjectPicker::released,
            this, &PoseRenderable::released);
    connect(m_picker, &Qt3DRender::QObjectPicker::entered,
            this, &PoseRenderable::entered);
    connect(m_picker, &Qt3DRender::QObjectPicker::exited,
            this, &PoseRenderable::exited);
}

QString PoseRenderable::poseID() {
    return m_pose.id();
}

ObjectModel PoseRenderable::objectModel() const {
    return m_pose.objectModel();
}

Pose PoseRenderable::pose() const {
    return m_pose;
}

Qt3DCore::QTransform *PoseRenderable::transform() const {
    return m_transform;
}

void PoseRenderable::setTranslation(const QVector3D &translation) {
    m_transform->setTranslation(translation);
}

void PoseRenderable::setRotation(const QQuaternion &rotation) {
    m_transform->setRotation(rotation);
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return m_pose.id() == other.m_pose.id();
}
