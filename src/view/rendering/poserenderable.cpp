#include "poserenderable.hpp"

PoseRenderable::PoseRenderable(Qt3DCore::QEntity *parent,
                               PosePtr pose) :
        ObjectModelRenderable(parent, *pose->objectModel()),
        m_pose(pose),
        m_picker(new Qt3DRender::QObjectPicker),
        m_transform(new Qt3DCore::QTransform) {
    m_transform->setRotation(pose->rotation());
    m_transform->setTranslation(pose->position());
    addComponent(m_transform);
    addComponent(m_picker);
    m_picker->setHoverEnabled(true);
    m_picker->setDragEnabled(true);
    connect(m_picker, &Qt3DRender::QObjectPicker::clicked,
            this, &PoseRenderable::clicked);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &PoseRenderable::moved);
    connect(pose.get(), &Pose::positionChanged,
            m_transform, &Qt3DCore::QTransform::setTranslation);
    connect(pose.get(), &Pose::rotationChanged,
            m_transform, &Qt3DCore::QTransform::setRotation);
}

ObjectModelPtr PoseRenderable::objectModel() {
    return m_pose->objectModel();
}

QString PoseRenderable::poseID() {
    return m_pose->id();
}

bool PoseRenderable::operator==(const PoseRenderable &other) {
    return m_pose->id() == other.m_pose->id();
}

PosePtr PoseRenderable::getPose() const {
    return m_pose;
}

Qt3DCore::QTransform *PoseRenderable::getTransform() const {
    return m_transform;
}
