#include "pose.hpp"

Pose::Pose() {

}

Pose::Pose(QString id,
           QVector3D position,
           QQuaternion rotation,
           const Image &image,
           const ObjectModel &objectModel)
    : m_position(position),
      m_rotation(rotation),
      m_image(image),
      m_objectModel(objectModel),
      m_id(id) {
}

Pose::Pose(QString id,
           QVector3D position,
           QMatrix3x3 rotation,
           const Image &image,
           const ObjectModel &objectModel)
    : Pose(id, position, QQuaternion::fromRotationMatrix(rotation), image, objectModel) {
}

Pose::Pose(const Pose &other)
    : QObject(),
      m_position(other.m_position),
      m_rotation(other.m_rotation),
      m_image(other.m_image),
      m_objectModel(other.m_objectModel),
      m_id(other.m_id) {
}

QVector3D Pose::position() const {
    return m_position;
}

QQuaternion Pose::rotation() const {
    return m_rotation;
}

Image Pose::image() const {
    return m_image;
}

ObjectModel Pose::objectModel() const {
    return m_objectModel;
}

void Pose::setPosition(const QVector3D &position) {
    this->m_position = position;
}

void Pose::setRotation(const QMatrix3x3 &rotation) {
    this->m_rotation = QQuaternion::fromRotationMatrix(rotation);
}

void Pose::setRotation(const QQuaternion &rotation) {
    this->m_rotation = rotation;
}

QString Pose::id() const {
    return m_id;
}

bool Pose::operator==(const Pose& objectImagePose) const {
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
