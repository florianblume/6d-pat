#include "objectimagecorrespondence.hpp"

ObjectImageCorrespondence::ObjectImageCorrespondence(QString id,
                                                     QVector3D position,
                                                     QVector3D rotation,
                                                     const Image* image,
                                                     const ObjectModel* objectModel)
    : image(image),
      objectModel(objectModel),
      position(position),
      rotation(rotation),
      id(id) {
}

ObjectImageCorrespondence::ObjectImageCorrespondence(const ObjectImageCorrespondence &other)
    : position(other.position),
      rotation(other.rotation),
      image(other.image),
      objectModel(other.objectModel),
      id(other.id) {
}

QVector3D ObjectImageCorrespondence::getPosition() const {
    return position;
}

QVector3D ObjectImageCorrespondence::getRotation() const {
    return rotation;
}

const Image* ObjectImageCorrespondence::getImage() const {
    return image;
}

const ObjectModel* ObjectImageCorrespondence::getObjectModel() const {
    return objectModel;
}

void ObjectImageCorrespondence::setPosition(QVector3D position) {
    this->position = position;
}

void ObjectImageCorrespondence::setRotation(QVector3D rotation) {
    this->rotation = std::move(rotation);
}

QString ObjectImageCorrespondence::getID() const {
    return id;
}

bool ObjectImageCorrespondence::operator==(const ObjectImageCorrespondence& objectImageCorrespondence) {
    return id.compare(objectImageCorrespondence.getID()) == 0;
}

ObjectImageCorrespondence& ObjectImageCorrespondence::operator=(const ObjectImageCorrespondence &other) {
    id = other.id;
    image = other.image;
    objectModel = other.objectModel;
    position = other.position;
    rotation = other.rotation;
    return *this;
}

QString ObjectImageCorrespondence::toString() const {
    return id + " " + image->getImagePath() + " " + objectModel->getPath() + " " + QString::number(position.x()) + " " +
            QString::number(position.y()) + " " + QString::number(position.z()) + " " + QString::number(rotation.x()) + " " +
            QString::number(rotation.y()) + " " + QString::number(rotation.z());
}
