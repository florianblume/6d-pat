#include "modelmanager.h"

ModelManager::ModelManager() {

}

void ModelManager::init() {
    readSettings();
    load();
}

ModelManager::~ModelManager() {
    writeSettings();
}

//! This function persistently stores settings of the application.
void ModelManager::writeSettings() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("ModelManager");
    settings.setValue("imagesPath", imagesPath.c_str());
    settings.setValue("objectModelsPath", objectModelsPath.c_str());
    settings.setValue("correspondencesPath", correspondencesPath.c_str());
    settings.endGroup();
}

//! This function persistently stores settings of the application.
void ModelManager::readSettings() {
    QSettings settings("FlorettiKonfetti Inc.", "Otiat");

    settings.beginGroup("ModelManager");
    imagesPath = settings.value("imagesPath").toString().toStdString();
    objectModelsPath = settings.value("objectModelsPath").toString().toStdString();
    correspondencesPath = settings.value("corresopndencesPath").toString().toStdString();
    settings.endGroup();
}

void ModelManager::load() {
    // TODO: suboptimal
    loadAndStoreStrategy.loadEntities(imagesPath, &images, objectModelsPath, &objectModels, correspondencesPath, &correspondences);
}

bool ModelManager::setImagesPath(string path) {
    if (!loadAndStoreStrategy.pathExists(path))
        return false;
    imagesPath = path;
    load();
    return true;
}

string ModelManager::getImagesPath() const {
    return  imagesPath;
}

bool ModelManager::setObjectModelsPath(string path) {
    if (!loadAndStoreStrategy.pathExists(path))
        return false;
    objectModelsPath = path;
    load();
    return true;
}

string ModelManager::getObjectModelsPath() const {
    return objectModelsPath;
}

bool ModelManager::setCorrespondencesPath(string path) {
    if (!loadAndStoreStrategy.pathExists(path))
        return false;
    correspondencesPath = path;
    load();
    return true;
}

string ModelManager::getCorrespondencesPath() const {
    return correspondencesPath;
}

void ModelManager::setSegmentationImagePattern(string pattern) {
    string test = pattern;
}

string ModelManager::getSegmentationImagePattern() {
    return "";
}

const list<Image>* ModelManager::getImages() const {
    return &images;
}

list<ObjectImageCorrespondence*> ModelManager::getCorrespondencesForImage(string imagePath) const {
    if (correspondencesForImages.find(imagePath) == correspondencesForImages.end()) {
        return list<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForImages.at(imagePath);
    }
}

const list<ObjectModel>* ModelManager::getObjectModels() const {
    return &objectModels;
}

const list<ObjectImageCorrespondence*> ModelManager::getCorrespondencesForObjectModels(string objectModelPath) const{
    if (correspondencesForObjectModels.find(objectModelPath) == correspondencesForObjectModels.end()) {
        return list<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForObjectModels.at(objectModelPath);
    }
}

const list<ObjectImageCorrespondence>* ModelManager::getCorrespondences() const {
    return &correspondences;
}

bool ModelManager::addOrUpdateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(objectImageCorrespondence, correspondencesPath, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    list<ObjectImageCorrespondence>::iterator it = find(correspondences.begin(), correspondences.end(), objectImageCorrespondence);
    const string imagePath = objectImageCorrespondence.getImage()->getPath();
    const string objectModelPath = objectImageCorrespondence.getObjectModel()->getPath();

    if (it == correspondences.end()) {
        //! correspondence has not yet been added
        correspondences.push_back(objectImageCorrespondence);

        //! initialize and add the list of correspondences for an image
        list<ObjectImageCorrespondence*> newCorrespondencesForImageList = list<ObjectImageCorrespondence*>();
        newCorrespondencesForImageList.push_back(&objectImageCorrespondence);
        correspondencesForImages[imagePath] = newCorrespondencesForImageList;

        //! initialize and add the list of correspondences for an object model
        list<ObjectImageCorrespondence*> newCorrespondencesForObjectModelList = list<ObjectImageCorrespondence*>();
        newCorrespondencesForObjectModelList.push_back(&objectImageCorrespondence);
        correspondencesForObjectModels[objectModelPath] = newCorrespondencesForObjectModelList;
    } else {
        //! correspondence already exist so we just have to update it
        //! correspondences in the list of correspondences for image or object models will be updated because they simply
        //! reference the "main" correspondence
        const Point* position = objectImageCorrespondence.getPosition();
        const Point* rotation = objectImageCorrespondence.getRotation();
        (*it).setPosition(position->x, position->y, position->z);
        (*it).setRotation(rotation->x, rotation->y, rotation->z);
    }

    return true;
}

bool ModelManager::removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(objectImageCorrespondence, correspondencesPath, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the correspondence didn't exist
        //! thus it doesn't make sense to remove the correspondence from this manager
        return false;
    }

    correspondences.remove(objectImageCorrespondence);
    correspondencesForImages.erase(objectImageCorrespondence.getImage()->getPath());
    correspondencesForObjectModels.erase(objectImageCorrespondence.getObjectModel()->getPath());
    return true;
}
