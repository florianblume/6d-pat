#include "pose.hpp"

Pose::Pose(QString id,
            QVector3D position,
            QMatrix3x3 rotation,
            ImagePtr image,
            ObjectModelPtr objectModel)
    : m_position(position),
      m_rotation(rotation),
      m_image(image),
      m_objectModel(objectModel),
      m_id(id) {
}

Pose::Pose(const Pose &other)
    : m_position(other.m_position),
      m_rotation(other.m_rotation),
      m_image(other.m_image),
      m_objectModel(other.m_objectModel),
      m_id(other.m_id) {
}

QVector3D Pose::position() const {
    return m_position;
}

QMatrix3x3 Pose::rotation() const {
    return m_rotation;
}

ImagePtr Pose::image() const {
    return m_image;
}

ObjectModelPtr Pose::objectModel() const {
    return m_objectModel;
}

void Pose::setPosition(const QVector3D &position) {
    this->m_position = position;
}

void Pose::setRotation(const QMatrix3x3 &rotation) {
    this->m_rotation = std::move(rotation);
}

QString Pose::id() const {
    return m_id;
}

bool Pose::operator==(const Pose& objectImagePose) {
    return m_id.compare(objectImagePose.id()) == 0;
}

Pose& Pose::operator=(const Pose &other) {
    m_id = other.m_id;
    m_image = other.m_image;
    m_objectModel = other.m_objectModel;
    m_position = other.m_position;
    m_rotation = other.m_rotation;
    return *this;
}
