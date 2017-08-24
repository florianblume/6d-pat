#include "objectimagecorrespondence.hpp"

ObjectImageCorrespondence::ObjectImageCorrespondence(string _id, int x, int y, int z, int r1, int r2, int r3, float articulation,
                                                     const Image* _image, const ObjectModel* _objectModel)
    : image(_image), objectModel(_objectModel), id(_id) {
    position = {x, y, z};
    rotation = {r1, r2, r3};
    this->articulation = articulation;
    accepted = false;
}

const Point* ObjectImageCorrespondence::getPosition() const {
    return &position;
}

const Point* ObjectImageCorrespondence::getRotation() const {
    return &rotation;
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

void ObjectImageCorrespondence::setPosition(int x, int y, int z) {
    position = {x, y, z};
}

void ObjectImageCorrespondence::setRotation(int r1, int r2, int r3) {
    rotation = {r1, r2, r3};
}

void ObjectImageCorrespondence::setArticulation(float articulation) {
    this->articulation = articulation;
}

const string ObjectImageCorrespondence::getID() const {
    return id;
}

bool ObjectImageCorrespondence::operator==(ObjectImageCorrespondence objectImageCorrespondence) {
    return strcmp(id.c_str(), objectImageCorrespondence.getID().c_str()) == 0;
}

string ObjectImageCorrespondence::toString() const {
    return id + " " + image->getImagePath().string() + " " + objectModel->getPath().string() + " " + to_string(position.x) + " " +
            to_string(position.y) + " " + to_string(position.z) + " " + to_string(rotation.x) + " " +
            to_string(rotation.y) + " " + to_string(rotation.z) + " " + to_string(articulation);
}
