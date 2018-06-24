#include "correspondence.hpp"

Correspondence::Correspondence(QString id,
                                                     QVector3D position,
                                                     QMatrix3x3 rotation,
                                                     const Image* image,
                                                     const ObjectModel* objectModel)
    : position(position),
      rotation(rotation),
      image(image),
      objectModel(objectModel),
      id(id) {
}

Correspondence::Correspondence(const Correspondence &other)
    : position(other.position),
      rotation(other.rotation),
      image(other.image),
      objectModel(other.objectModel),
      id(other.id) {
}

QVector3D Correspondence::getPosition() const {
    return position;
}

QMatrix3x3 Correspondence::getRotation() const {
    return rotation;
}

const Image* Correspondence::getImage() const {
    return image;
}

const ObjectModel* Correspondence::getObjectModel() const {
    return objectModel;
}

void Correspondence::setPosition(QVector3D position) {
    this->position = position;
}

void Correspondence::setRotation(QMatrix3x3 rotation) {
    this->rotation = std::move(rotation);
}

QString Correspondence::getID() const {
    return id;
}

bool Correspondence::operator==(const Correspondence& objectImageCorrespondence) {
    return id.compare(objectImageCorrespondence.getID()) == 0;
}

Correspondence& Correspondence::operator=(const Correspondence &other) {
    id = other.id;
    image = other.image;
    objectModel = other.objectModel;
    position = other.position;
    rotation = other.rotation;
    return *this;
}
