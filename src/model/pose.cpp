#include "pose.hpp"

Pose::Pose(QString id,
            QVector3D position,
            QMatrix3x3 rotation,
            ImagePtr image,
            ObjectModelPtr objectModel)
    : position(position),
      rotation(rotation),
      image(image),
      objectModel(objectModel),
      id(id) {
}

Pose::Pose(const Pose &other)
    : position(other.position),
      rotation(other.rotation),
      image(other.image),
      objectModel(other.objectModel),
      id(other.id) {
}

QVector3D Pose::getPosition() const {
    return position;
}

QMatrix3x3 Pose::getRotation() const {
    return rotation;
}

ImagePtr Pose::getImage() const {
    return image;
}

ObjectModelPtr Pose::getObjectModel() const {
    return objectModel;
}

void Pose::setPosition(const QVector3D &position) {
    this->position = position;
}

void Pose::setRotation(const QMatrix3x3 &rotation) {
    this->rotation = std::move(rotation);
}

QString Pose::getID() const {
    return id;
}

bool Pose::operator==(const Pose& objectImagePose) {
    return id.compare(objectImagePose.getID()) == 0;
}

Pose& Pose::operator=(const Pose &other) {
    id = other.id;
    image = other.image;
    objectModel = other.objectModel;
    position = other.position;
    rotation = other.rotation;
    return *this;
}
