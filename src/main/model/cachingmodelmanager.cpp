#include "cachingmodelmanager.hpp"
#include <QUuid>

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    images = loadAndStoreStrategy.loadImages();
    objectModels = loadAndStoreStrategy.loadObjectModels();
    correspondences = loadAndStoreStrategy.loadCorrespondences(images, objectModels);
    createConditionalCache();

    connect(&loadAndStoreStrategy, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
    connect(&loadAndStoreStrategy, SIGNAL(imagesPathChanged()),
            this, SLOT(onImagesChanged()));
    connect(&loadAndStoreStrategy, SIGNAL(objectModelsPathChanged()),
            this, SLOT(onObjectModelsChanged()));
    connect(&loadAndStoreStrategy, SIGNAL(correspondencesPathChanged()),
            this, SLOT(onCorrespondencesChanged()));
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::createConditionalCache() {
    correspondencesForImages.clear();
    correspondencesForObjectModels.clear();
    for (int i = 0; i < correspondences.size(); i++) {
        ObjectImageCorrespondence &correspondence = correspondences[i];

        //! Setup cache of correspondences that can be retrieved via an image
        QList<ObjectImageCorrespondence> &correspondencesForImage =
                correspondencesForImages[correspondence.getImage()->getImagePath()];
        correspondencesForImage.append(correspondence);

        //! Setup cache of correspondences that can be retrieved via an object model
        QList<ObjectImageCorrespondence> &correspondencesForObjectModel =
                correspondencesForObjectModels[correspondence.getObjectModel()->getPath()];
        correspondencesForObjectModel.append(correspondence);
    }
}

QList<Image> CachingModelManager::getImages() const {
    return images;
}

QList<ObjectImageCorrespondence> CachingModelManager::getCorrespondencesForImage(const Image &image) const  {
    if (correspondencesForImages.find(image.getImagePath()) != correspondencesForImages.end()) {
        return correspondencesForImages[image.getImagePath()];
    }

    return QList<ObjectImageCorrespondence>();
}

QList<ObjectModel> CachingModelManager::getObjectModels() const {
    return objectModels;
}

QList<ObjectImageCorrespondence> CachingModelManager::getCorrespondencesForObjectModel(const ObjectModel &objectModel) {
    if (correspondencesForObjectModels.find(objectModel.getPath()) != correspondencesForObjectModels.end()) {
        return correspondencesForObjectModels[objectModel.getPath()];
    }

    return QList<ObjectImageCorrespondence>();
}

QList<ObjectImageCorrespondence> CachingModelManager::getCorrespondences() {
    return correspondences;
}

QList<ObjectImageCorrespondence> CachingModelManager::getCorrespondencesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) {
    QList<ObjectImageCorrespondence> correspondencesForImageAndObjectModel;
    for (ObjectImageCorrespondence &correspondence : correspondencesForImages[image.getImagePath()]) {
        if (correspondence.getObjectModel()->getPath().compare(objectModel.getPath()) == 0) {
           correspondencesForImageAndObjectModel.append(correspondence);
        }
    }
    return correspondencesForImageAndObjectModel;
}

bool CachingModelManager::addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    const QVector3D position = objectImageCorrespondence.getPosition();
    const QVector3D rotation = objectImageCorrespondence.getRotation();
    float articulation = objectImageCorrespondence.getArticulation();
    const Image* image = objectImageCorrespondence.getImage();
    const ObjectModel* model = objectImageCorrespondence.getObjectModel();
    ObjectImageCorrespondence correspondence(QUuid::createUuid().toString(),
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

    const QString imagePath = correspondence.getImage()->getImagePath();
    const QString objectModelPath = correspondence.getObjectModel()->getPath();

    //! initialize and add the list of correspondences for an image
    QList<ObjectImageCorrespondence> newCorrespondencesForImageList;
    newCorrespondencesForImageList.push_back(correspondences.front());
    correspondencesForImages[imagePath] = std::move(newCorrespondencesForImageList);

    //! initialize and add the list of correspondences for an object model
    QList<ObjectImageCorrespondence> newCorrespondencesForObjectModelList;
    newCorrespondencesForObjectModelList.push_back(correspondences.front());
    correspondencesForObjectModels[objectModelPath] = std::move(newCorrespondencesForObjectModelList);

    emit correspondencesChanged();

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

    emit correspondencesChanged();

    return true;
}

bool CachingModelManager::removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) {
    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(objectImageCorrespondence, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the correspondence didn't exist
        //! thus it doesn't make sense to remove the correspondence from this manager
        return false;
    }

    bool correspondenceFound = false;
    for (int i = 0; i < correspondences.size(); i++) {
        if (correspondences[i].getID().compare(objectImageCorrespondence.getID()) == 0) {
            correspondences.erase(correspondences.begin() + i);
            correspondenceFound = true;
        }
    }

    if (!correspondenceFound)
        //! if we didn't find a correspondence there is no need to proceed any further, maybe this manager is not managing the given correspondence
        return false;

    QString imagePath = objectImageCorrespondence.getImage()->getImagePath();
    QList<ObjectImageCorrespondence>& correspondencesForImage = correspondencesForImages[imagePath];
    for (int i = 0; i < correspondencesForImage.size(); i++) {
        if (correspondencesForImage[i].getImage()->getImagePath().compare(imagePath)) {
            correspondencesForImage.erase(correspondencesForImage.begin() + i);
        }
    }

    QString objectModelPath = objectImageCorrespondence.getObjectModel()->getPath();
    QList<ObjectImageCorrespondence>& correspondencesForObject = correspondencesForObjectModels[objectModelPath];
    for (int i = 0; i < correspondencesForObject.size(); i++) {
        if (correspondencesForObject[i].getObjectModel()->getPath().compare(objectModelPath)) {
            correspondencesForObject.erase(correspondencesForObject.begin() + i);
        }
    }

    emit correspondencesChanged();

    return true;
}

void CachingModelManager::onImagesChanged() {
    images = loadAndStoreStrategy.loadImages();
    correspondences = loadAndStoreStrategy.loadCorrespondences(images, objectModels);
    createConditionalCache();
    emit imagesChanged();
    emit correspondencesChanged();
}

void CachingModelManager::onObjectModelsChanged() {
    objectModels = loadAndStoreStrategy.loadObjectModels();
    correspondences = loadAndStoreStrategy.loadCorrespondences(images, objectModels);
    createConditionalCache();
    emit objectModelsChanged();
    emit correspondencesChanged();
}

void CachingModelManager::onCorrespondencesChanged() {
    correspondences = loadAndStoreStrategy.loadCorrespondences(images, objectModels);
    createConditionalCache();
    emit correspondencesChanged();
}
