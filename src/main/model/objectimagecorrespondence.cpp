#include "objectimagecorrespondence.hpp"

ObjectImageCorrespondence::ObjectImageCorrespondence(QString id,
                                                     float x, float y, float z,
                                                     float r1, float r2, float r3,
                                                     float articulation,
                                                     const Image* image,
                                                     const ObjectModel* objectModel)
    : image(image),
      objectModel(objectModel),
      id(id) {
    position = {x, y, z};
    rotation = {r1, r2, r3};
    this->articulation = articulation;
    accepted = false;
}

ObjectImageCorrespondence::ObjectImageCorrespondence(const ObjectImageCorrespondence &other)
    : position(other.position),
      rotation(other.rotation),
      articulation(other.articulation),
      accepted(other.accepted),
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

float ObjectImageCorrespondence::getArticulation() const {
    return articulation;
}

const Image* ObjectImageCorrespondence::getImage() const {
    return image;
}

const ObjectModel* ObjectImageCorrespondence::getObjectModel() const {
    return objectModel;
}

void ObjectImageCorrespondence::setPosition(float x, float y, float z) {
    position = {x, y, z};
}

void ObjectImageCorrespondence::setPosition(QVector3D position) {
    this->position = std::move(position);
}

void ObjectImageCorrespondence::setRotation(float r1, float r2, float r3) {
    rotation = {r1, r2, r3};
}

void ObjectImageCorrespondence::setRotation(QVector3D rotation) {
    this->rotation = std::move(rotation);
}

void ObjectImageCorrespondence::setArticulation(float articulation) {
    this->articulation = articulation;
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
    articulation = other.articulation;
    accepted = other.accepted;
    return *this;
}

QString ObjectImageCorrespondence::toString() const {
    return id + " " + image->getImagePath() + " " + objectModel->getPath() + " " + QString::number(position.x()) + " " +
            QString::number(position.y()) + " " + QString::number(position.z()) + " " + QString::number(rotation.x()) + " " +
            QString::number(rotation.y()) + " " + QString::number(rotation.z()) + " " + QString::number(articulation);
}
