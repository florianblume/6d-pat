#include "objectimagecorrespondence.hpp"

ObjectImageCorrespondence::ObjectImageCorrespondence(QString id,
                                                     float x, float y, float z,
                                                     float r1, float r2, float r3,
                                                     float articulation,
                                                     const Image& image,
                                                     const ObjectModel& objectModel)
    : image(image),
      objectModel(objectModel),
      id(id) {
    position = {x, y, z};
    rotation = {r1, r2, r3};
    this->articulation = articulation;
    accepted = false;
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

const Image& ObjectImageCorrespondence::getImage() const {
    return image;
}

const ObjectModel& ObjectImageCorrespondence::getObjectModel() const {
    return objectModel;
}

void ObjectImageCorrespondence::setPosition(float x, float y, float z) {
    position = {x, y, z};
}

void ObjectImageCorrespondence::setRotation(float r1, float r2, float r3) {
    rotation = {r1, r2, r3};
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

QString ObjectImageCorrespondence::toString() const {
    return id + " " + image->getImagePath() + " " + objectModel->getPath() + " " + QString::number(position.x()) + " " +
            QString::number(position.y()) + " " + QString::number(position.z()) + " " + QString::number(rotation.x()) + " " +
            QString::number(rotation.y()) + " " + QString::number(rotation.z()) + " " + QString::number(articulation);
}
