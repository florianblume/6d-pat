#include "cachingmodelmanager.h"

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& _loadAndStoreStrategy) : ModelManager (_loadAndStoreStrategy) {
    images = loadAndStoreStrategy->loadImages();
    objectModels = loadAndStoreStrategy->loadObjectModels();
    correspondences = loadAndStoreStrategy->loadCorrespondences(&images, &objectModels);
}

CachingModelManager::~CachingModelManager() {
}

const list<Image>* CachingModelManager::getImages() const {
    return &images;
}

list<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForImage(string imagePath) const {
    if (correspondencesForImages.find(imagePath) == correspondencesForImages.end()) {
        return list<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForImages.at(imagePath);
    }
}

const list<ObjectModel>* CachingModelManager::getObjectModels() const {
    return &objectModels;
}

const list<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForObjectModel(string objectModelPath) const{
    if (correspondencesForObjectModels.find(objectModelPath) == correspondencesForObjectModels.end()) {
        return list<ObjectImageCorrespondence*>();
    } else {
        return correspondencesForObjectModels.at(objectModelPath);
    }
}

const list<ObjectImageCorrespondence>* CachingModelManager::getCorrespondences() const {
    return &correspondences;
}

const list<ObjectImageCorrespondence*> CachingModelManager::getCorrespondencesForImageAndObjectModel(string imagePath, string objectModelPath) {
    list<ObjectImageCorrespondence*> correspondencesForImageAndObject = correspondencesForImages[imagePath];
    correspondencesForImageAndObject.remove_if([&objectModelPath] (const ObjectImageCorrespondence* correspondence) {
        return !correspondence->getObjectModel()->getPath().compare(objectModelPath);
    });
    return correspondencesForImageAndObject;
}

bool CachingModelManager::addObjectImageCorrespondence(Image* image, ObjectModel* objectModel, Point position, Point rotation) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    ObjectImageCorrespondence corresopndence(boost::lexical_cast<std::string>(uuid), position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, image, objectModel);

    if (!loadAndStoreStrategy->persistObjectImageCorrespondence(corresopndence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence has not yet been added
    correspondences.push_back(corresopndence);

    const string imagePath = corresopndence.getImage()->getPath();
    const string objectModelPath = corresopndence.getObjectModel()->getPath();

    //! initialize and add the list of correspondences for an image
    list<ObjectImageCorrespondence*> newCorrespondencesForImageList = list<ObjectImageCorrespondence*>();
    newCorrespondencesForImageList.push_back(&corresopndence);
    correspondencesForImages[imagePath] = newCorrespondencesForImageList;

    //! initialize and add the list of correspondences for an object model
    list<ObjectImageCorrespondence*> newCorrespondencesForObjectModelList = list<ObjectImageCorrespondence*>();
    newCorrespondencesForObjectModelList.push_back(&corresopndence);
    correspondencesForObjectModels[objectModelPath] = newCorrespondencesForObjectModelList;

    return true;
}

bool CachingModelManager::updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    list<ObjectImageCorrespondence>::iterator it = find(correspondences.begin(), correspondences.end(), objectImageCorrespondence);

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

    return true;
}

bool CachingModelManager::removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy->persistObjectImageCorrespondence(objectImageCorrespondence, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the correspondence didn't exist
        //! thus it doesn't make sense to remove the correspondence from this manager
        return false;
    }

    correspondences.remove(objectImageCorrespondence);
    correspondencesForImages.erase(objectImageCorrespondence.getImage()->getPath());
    correspondencesForObjectModels.erase(objectImageCorrespondence.getObjectModel()->getPath());
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
