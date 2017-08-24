#include "cachingmodelmanager.hpp"
#include "../misc/otiathelper.h"

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& _loadAndStoreStrategy) : ModelManager (_loadAndStoreStrategy) {
    images = loadAndStoreStrategy->loadImages();
    objectModels = loadAndStoreStrategy->loadObjectModels();
    correspondences = loadAndStoreStrategy->loadCorrespondences(&images, &objectModels);
}

CachingModelManager::~CachingModelManager() {
}

vector<Image>* CachingModelManager::getImages() {
    return &images;
}

vector<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForImage(string imagePath) {
    if (correspondencesForImages.find(imagePath) == correspondencesForImages.end()) {
        return vector<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForImages.at(imagePath);
    }
}

vector<ObjectModel>* CachingModelManager::getObjectModels() {
    return &objectModels;
}

vector<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForObjectModel(string objectModelPath) {
    if (correspondencesForObjectModels.find(objectModelPath) == correspondencesForObjectModels.end()) {
        return vector<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForObjectModels.at(objectModelPath);
    }
}

vector<ObjectImageCorrespondence>* CachingModelManager::getCorrespondences() {
    return &correspondences;
}

vector<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForImageAndObjectModel(string imagePath, string objectModelPath) {
    vector<ObjectImageCorrespondence*> correspondencesForImageAndObject = correspondencesForImages[imagePath];
    //! the part below removes all correspondences that do not have the given object model path
    for (uint i = 0; i < correspondencesForImageAndObject.size(); i++) {
        if (correspondencesForImageAndObject[i]->getObjectModel()->getPath().string().compare(objectModelPath)) {
            correspondencesForImageAndObject.erase(correspondencesForImageAndObject.begin() + i);
        }
    }
    return correspondencesForImageAndObject;
}

bool CachingModelManager::addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    const Point* position = objectImageCorrespondence.getPosition();
    const Point* rotation = objectImageCorrespondence.getRotation();
    float articulation = objectImageCorrespondence.getArticulation();
    const Image* image = objectImageCorrespondence.getImage();
    const ObjectModel* model = objectImageCorrespondence.getObjectModel();
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    ObjectImageCorrespondence correspondence(boost::uuids::to_string(uuid),
                                             position->x, position->y, position->z, rotation->x, rotation->y, rotation->z,
                                             articulation,
                                             image, model);

    if (!loadAndStoreStrategy->persistObjectImageCorrespondence(correspondence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence has not yet been added
    correspondences.push_back(correspondence);

    const string imagePath = correspondence.getImage()->getImagePath().string();
    const string objectModelPath = correspondence.getObjectModel()->getPath().string();

    //! initialize and add the list of correspondences for an image
    vector<ObjectImageCorrespondence*> newCorrespondencesForImageList = vector<ObjectImageCorrespondence*>();
    newCorrespondencesForImageList.push_back(&correspondences.front());
    correspondencesForImages[imagePath] = newCorrespondencesForImageList;

    //! initialize and add the list of correspondences for an object model
    vector<ObjectImageCorrespondence*> newCorrespondencesForObjectModelList = vector<ObjectImageCorrespondence*>();
    newCorrespondencesForObjectModelList.push_back(&correspondences.front());
    correspondencesForObjectModels[objectModelPath] = newCorrespondencesForObjectModelList;

    return true;
}

bool CachingModelManager::updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    vector<ObjectImageCorrespondence>::iterator it = find(correspondences.begin(), correspondences.end(), objectImageCorrespondence);

    if (it == correspondences.end()) {
        //! this manager does not manager the given correspondence
        return false;
    }

    if (!loadAndStoreStrategy->persistObjectImageCorrespondence(objectImageCorrespondence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence already exist so we just have to update it
    //! correspondences in the list of correspondences for image or object models will be updated because they simply
    //! reference the "main" correspondence
    const Point* position = objectImageCorrespondence.getPosition();
    const Point* rotation = objectImageCorrespondence.getRotation();
    (*it).setPosition(position->x, position->y, position->z);
    (*it).setRotation(rotation->x, rotation->y, rotation->z);
    (*it).setArticulation(objectImageCorrespondence.getArticulation());

    return true;
}

bool CachingModelManager::removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy->persistObjectImageCorrespondence(objectImageCorrespondence, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the correspondence didn't exist
        //! thus it doesn't make sense to remove the correspondence from this manager
        return false;
    }

    bool correspondenceFound = false;
    for (uint i = 0; i < correspondences.size(); i++) {
        if (correspondences[i].getID().compare(objectImageCorrespondence.getID()) == 0) {
            correspondences.erase(correspondences.begin() + i);
            correspondenceFound = true;
        }
    }

    if (!correspondenceFound)
        //! if we didn't find a correspondence there is no need to proceed any further, maybe this manager is not managing the given correspondence
        return false;

    string imagePath = objectImageCorrespondence.getImage()->getImagePath().string();
    vector<ObjectImageCorrespondence*>& correspondencesForImage = correspondencesForImages[imagePath];
    for (uint i = 0; i < correspondencesForImage.size(); i++) {
        if (correspondencesForImage[i]->getImage()->getImagePath().string().compare(objectImageCorrespondence.getImage()->getImagePath().string())) {
            correspondencesForImage.erase(correspondencesForImage.begin() + i);
        }
    }

    string objectModelPath = objectImageCorrespondence.getObjectModel()->getPath().string();
    vector<ObjectImageCorrespondence*>& correspondencesForObject = correspondencesForObjectModels[objectModelPath];
    for (uint i = 0; i < correspondencesForObject.size(); i++) {
        if (correspondencesForObject[i]->getObjectModel()->getPath().string().compare(objectModelPath)) {
            correspondencesForObject.erase(correspondencesForObject.begin() + i);
        }
    }

    return true;
}

void CachingModelManager::imagesChanged() {
    images = loadAndStoreStrategy->loadImages();
}

void CachingModelManager::objectModelsChanged() {
    objectModels = loadAndStoreStrategy->loadObjectModels();
}

void CachingModelManager::corresopndencesChanged() {
    correspondences = loadAndStoreStrategy->loadCorrespondences(&images, &objectModels);
}
