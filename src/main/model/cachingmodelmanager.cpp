#include "cachingmodelmanager.hpp"
#include <QUuid>

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    loadAndStoreStrategy.loadImages(images);
    loadAndStoreStrategy.loadObjectModels(objectModels);
    loadAndStoreStrategy.loadCorrespondences(images, objectModels, correspondences);
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::getImages(QList<const Image*> &images) const {
    for (const Image &image : this->images) {
        images.append(&image);
    }
}

const Image* CachingModelManager::getImage(uint index) const {
    return &images.at(index);
}

int CachingModelManager::getImagesSize() const {
    return images.size();
}

void CachingModelManager::getCorrespondencesForImage(const Image &image, QList<ObjectImageCorrespondence*> &correspondences) const  {
    if (correspondencesForImages.find(image.getImagePath()) != correspondencesForImages.end()) {
        correspondences.append(correspondencesForImages[image.getImagePath()]);
    }
}

void CachingModelManager::getObjectModels(QList<const ObjectModel*> &objectModels) const {
    for (const ObjectModel &objectModel : this->objectModels) {
        objectModels.append(&objectModel);
    }
}

const ObjectModel* CachingModelManager::getObjectModel(uint index) const {
    return &objectModels.at(index);
}

int CachingModelManager::getObjectModelsSize() const {
    return objectModels.size();
}

void CachingModelManager::getCorrespondencesForObjectModel(const ObjectModel &objectModel, QList<ObjectImageCorrespondence*> &corresopndenes) {
    if (correspondencesForObjectModels.find(objectModel.getPath()) != correspondencesForObjectModels.end()) {
        corresopndenes.append(correspondencesForObjectModels[objectModel.getPath()]);
    }
}

void CachingModelManager::getCorrespondences(QList<ObjectImageCorrespondence*> &correspondences) {
    for (ObjectImageCorrespondence &correspondence : this->correspondences) {
        correspondences.append(&correspondence);
    }
}

void CachingModelManager::getCorrespondencesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel, QList<ObjectImageCorrespondence*> &correspondences) {
    for (ObjectImageCorrespondence *correspondence : correspondencesForImages[image.getImagePath()]) {
        if (correspondence->getObjectModel().getPath().compare(objectModel.getPath()) == 0) {
           correspondences.append(correspondence);
        }
    }
}

bool CachingModelManager::addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    const QVector3D position = objectImageCorrespondence.getPosition();
    const QVector3D rotation = objectImageCorrespondence.getRotation();
    float articulation = objectImageCorrespondence.getArticulation();
    const Image& image = objectImageCorrespondence.getImage();
    const ObjectModel& model = objectImageCorrespondence.getObjectModel();
    QUuid uuid;
    ObjectImageCorrespondence correspondence(uuid.toString(),
                                             position.x(),
                                             position.y(),
                                             position.z(),
                                             rotation.x(),
                                             rotation.y(),
                                             rotation.z(),
                                             articulation,
                                             image, model);

    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(correspondence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence has not yet been added
    correspondences.push_back(correspondence);

    const QString imagePath = correspondence.getImage().getImagePath();
    const QString objectModelPath = correspondence.getObjectModel().getPath();

    //! initialize and add the list of correspondences for an image
    QList<ObjectImageCorrespondence*> newCorrespondencesForImageList;
    newCorrespondencesForImageList.push_back(&correspondences.front());
    correspondencesForImages[imagePath] = newCorrespondencesForImageList;

    //! initialize and add the list of correspondences for an object model
    QList<ObjectImageCorrespondence*> newCorrespondencesForObjectModelList;
    newCorrespondencesForObjectModelList.push_back(&correspondences.front());
    correspondencesForObjectModels[objectModelPath] = newCorrespondencesForObjectModelList;

    for (auto listener : listeners)
        listener->correspondenceAdded(objectImageCorrespondence.getID());

    return true;
}

bool CachingModelManager::updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    QList<ObjectImageCorrespondence>::iterator it = find(correspondences.begin(), correspondences.end(), objectImageCorrespondence);

    if (it == correspondences.end()) {
        //! this manager does not manager the given correspondence
        return false;
    }

    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(objectImageCorrespondence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence already exist so we just have to update it
    //! correspondences in the list of correspondences for image or object models will be updated because they simply
    //! reference the "main" correspondence
    const QVector3D position = objectImageCorrespondence.getPosition();
    const QVector3D rotation = objectImageCorrespondence.getRotation();
    (*it).setPosition(position.x(), position.y(), position.z());
    (*it).setRotation(rotation.x(), rotation.y(), rotation.z());
    (*it).setArticulation(objectImageCorrespondence.getArticulation());

    for (auto listener : listeners)
        listener->correspondenceUpdated(objectImageCorrespondence.getID());

    return true;
}

bool CachingModelManager::removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(objectImageCorrespondence, true)) {
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

    QString imagePath = objectImageCorrespondence.getImage().getImagePath();
    QList<ObjectImageCorrespondence*>& correspondencesForImage = correspondencesForImages[imagePath];
    for (uint i = 0; i < correspondencesForImage.size(); i++) {
        if (correspondencesForImage[i]->getImage().getImagePath().compare(imagePath)) {
            correspondencesForImage.erase(correspondencesForImage.begin() + i);
        }
    }

    QString objectModelPath = objectImageCorrespondence.getObjectModel().getPath();
    QList<ObjectImageCorrespondence*>& correspondencesForObject = correspondencesForObjectModels[objectModelPath];
    for (uint i = 0; i < correspondencesForObject.size(); i++) {
        if (correspondencesForObject[i]->getObjectModel().getPath().compare(objectModelPath)) {
            correspondencesForObject.erase(correspondencesForObject.begin() + i);
        }
    }

    for (auto listener : listeners)
        listener->correspondenceDeleted(objectImageCorrespondence.getID());

    return true;
}

void CachingModelManager::addListener(ModelManagerListener* listener) {
    listeners.push_back(listener);
}

void CachingModelManager::removeListener(ModelManagerListener* listener) {
    for (uint i = 0; i < listeners.size(); i++) {
        if (listeners[i] == listener) {
            listeners.erase(listeners.begin() + i);
        }
    }
}

void CachingModelManager::imagesChanged() {
    loadAndStoreStrategy.loadImages(images);
    for (auto listener : listeners)
        listener->imagesChanged();
}

void CachingModelManager::objectModelsChanged() {
    loadAndStoreStrategy.loadObjectModels(objectModels);
    for (auto listener : listeners)
        listener->objectModelsChanged();
}

void CachingModelManager::corresopndencesChanged() {
    loadAndStoreStrategy.loadCorrespondences(images, objectModels, correspondences);
    for (auto listener : listeners)
        listener->correspondencesChanged();
}
