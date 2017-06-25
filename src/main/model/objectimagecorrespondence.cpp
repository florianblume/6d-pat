#include "objectimagecorrespondence.h"

ObjectImageCorrespondence::ObjectImageCorrespondence(int x, int y, int z, int r1, int r2, int r3, Image *_image, ObjectModel *_objectModel)
    : image(_image), objectModel(_objectModel) {
    position = {x, y, z};
    rotation = {r1, r2, r3};
    accepted = false;
}

Point ObjectImageCorrespondence::getPosition() {
    return position;
}

Point ObjectImageCorrespondence::getRotation() {
    return rotation;
}

Image* ObjectImageCorrespondence::getImage() {
    return image;
}

ObjectModel* ObjectImageCorrespondence::getObjectModel() {
    return objectModel;
}

void ObjectImageCorrespondence::setPosition(int x, int y, int z) {
    position = {x, y, z};
}

void ObjectImageCorrespondence::setRotation(int r1, int r2, int r3) {
    rotation = {r1, r2, r3};
}
