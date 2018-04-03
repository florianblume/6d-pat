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

ObjectImageCorrespondence CachingModelManager::getCorrespondenceById(const QString &id) {
    auto itObj = std::find_if(
        correspondences.begin(), correspondences.end(),
        [id](ObjectImageCorrespondence o) { return o.getID() == id; }
    );
    if (itObj != correspondences.end()) {
        return *itObj;
    } else {
        return ObjectImageCorrespondence("", 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
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

bool CachingModelManager::addObjectImageCorrespondence(Image *image,
                                                       ObjectModel *objectModel,
                                                       QVector3D position,
                                                       QVector3D rotation,
                                                       float articulation,
                                                       bool accepted) {

    QList<Image>::iterator imageIterator = find(images.begin(), images.end(), *image);
    if (imageIterator == images.end())
        return false;

    QList<ObjectModel>::iterator objectModelIterator = find(objectModels.begin(), objectModels.end(), *objectModel);
    if (objectModelIterator == objectModels.end())
        return false;

    // IMPORTANT: Use the iterator values, they return the actually managed image and object model
    // and not what the user passed (and maybe created somewhere else but with the right paths)
    ObjectImageCorrespondence correspondence(QUuid::createUuid().toString(),
                                             position.x(),
                                             position.y(),
                                             position.z(),
                                             rotation.x(),
                                             rotation.y(),
                                             rotation.z(),
                                             articulation,
                                             &*imageIterator,
                                             &*objectModelIterator);
    // TODO: add accepted

    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(&correspondence, false)) {
        //! if there is an error persisting the correspondence for any reason we should not add the correspondence to this manager
        return false;
    }

    //! correspondence has not yet been added
    correspondences.push_back(correspondence);

    createConditionalCache();

    emit correspondenceAdded(correspondence.getID());

    return true;
}

bool CachingModelManager::updateObjectImageCorrespondence(const QString &id,
                                                          QVector3D position,
                                                          QVector3D rotation,
                                                          float articulation,
                                                          bool accepted) {
    ObjectImageCorrespondence *correspondence = Q_NULLPTR;
    for (int i = 0; i < correspondences.size(); i++) {
        if (correspondences[i].getID() == id)
            correspondence = &correspondences[i];
    }

    if (!correspondence) {
        //! this manager does not manager the given correspondence
        return false;
    }

    QVector3D previousPosition = correspondence->getPosition();
    QVector3D previousRotation = correspondence->getRotation();
    float previousArticulation = correspondence->getArticulation();

    correspondence->setPosition(position);
    correspondence->setRotation(rotation);
    correspondence->setArticulation(articulation);

    // TODO: set accepted

    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(correspondence, false)) {
        // if there is an error persisting the correspondence for any reason we should not keep the new values
        correspondence->setPosition(previousPosition);
        correspondence->setRotation(previousRotation);
        correspondence->setArticulation(previousArticulation);
        return false;
    }

    createConditionalCache();

    emit correspondenceUpdated(correspondence->getID());

    return true;
}

bool CachingModelManager::removeObjectImageCorrespondence(const QString &id) {
    ObjectImageCorrespondence *correspondence = Q_NULLPTR;
    for (int i = 0; i < correspondences.size(); i++) {
        if (correspondences[i].getID() == id)
            correspondence = &correspondences[i];
    }

    if (!correspondence) {
        //! this manager does not manager the given correspondence
        return false;
    }

    if (!loadAndStoreStrategy.persistObjectImageCorrespondence(correspondence, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the correspondence didn't exist
        //! thus it doesn't make sense to remove the correspondence from this manager
        return false;
    }

    for (int i = 0; i < correspondences.size(); i++) {
        if (correspondences.at(i).getID() == id)
        correspondences.removeAt(i);
    }

    createConditionalCache();

    emit correspondenceDeleted(id);

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
