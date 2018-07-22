#include "cachingmodelmanager.hpp"
#include "misc/generalhelper.h"

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    images = loadAndStoreStrategy.loadImages();
    objectModels = loadAndStoreStrategy.loadObjectModels();
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();

    connect(&loadAndStoreStrategy, SIGNAL(imagesChanged()),
            this, SLOT(onImagesChanged()));
    connect(&loadAndStoreStrategy, SIGNAL(objectModelsChanged()),
            this, SLOT(onObjectModelsChanged()));
    connect(&loadAndStoreStrategy, SIGNAL(posesChanged()),
            this, SLOT(onPosesChanged()));
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::createConditionalCache() {
    posesForImages.clear();
    posesForObjectModels.clear();
    for (int i = 0; i < poses.size(); i++) {
        Pose &pose = poses[i];

        //! Setup cache of poses that can be retrieved via an image
        QList<Pose> &posesForImage =
                posesForImages[pose.getImage()->getImagePath()];
        posesForImage.append(pose);

        //! Setup cache of poses that can be retrieved via an object model
        QList<Pose> &posesForObjectModel =
                posesForObjectModels[pose.getObjectModel()->getPath()];
        posesForObjectModel.append(pose);
    }
}

QList<Image> CachingModelManager::getImages() const {
    return images;
}

QList<Pose> CachingModelManager::getPosesForImage(const Image &image) const  {
    if (posesForImages.find(image.getImagePath()) != posesForImages.end()) {
        return posesForImages[image.getImagePath()];
    }

    return QList<Pose>();
}

QList<ObjectModel> CachingModelManager::getObjectModels() const {
    return objectModels;
}

QList<Pose> CachingModelManager::getPosesForObjectModel(const ObjectModel &objectModel) {
    if (posesForObjectModels.find(objectModel.getPath()) != posesForObjectModels.end()) {
        return posesForObjectModels[objectModel.getPath()];
    }

    return QList<Pose>();
}

QList<Pose> CachingModelManager::getPoses() {
    return poses;
}

QSharedPointer<Pose> CachingModelManager::getPoseById(const QString &id) {
    QSharedPointer<Pose> result;
    auto itObj = std::find_if(
        poses.begin(), poses.end(),
        [id](Pose o) { return o.getID() == id; }
    );
    if (itObj != poses.end()) {
        result.reset(new Pose(*itObj));
    }
    return result;
}

QList<Pose> CachingModelManager::getPosesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) {
    QList<Pose> posesForImageAndObjectModel;
    for (Pose &pose : posesForImages[image.getImagePath()]) {
        if (pose.getObjectModel()->getPath().compare(objectModel.getPath()) == 0) {
           posesForImageAndObjectModel.append(pose);
        }
    }
    return posesForImageAndObjectModel;
}

bool CachingModelManager::addObjectImagePose(Image *image,
                                                       ObjectModel *objectModel,
                                                       QVector3D position,
                                                       QMatrix3x3 rotation) {

    QList<Image>::iterator imageIterator = find(images.begin(), images.end(), *image);
    if (imageIterator == images.end())
        return false;

    QList<ObjectModel>::iterator objectModelIterator = find(objectModels.begin(), objectModels.end(), *objectModel);
    if (objectModelIterator == objectModels.end())
        return false;

    // IMPORTANT: Use the iterator values, they return the actually managed image and object model
    // and not what the user passed (and maybe created somewhere else but with the right paths)
    Image *_image = &*imageIterator;
    ObjectModel *_objectModel = &*objectModelIterator;
    Pose pose(GeneralHelper::createPoseId(_image, _objectModel),
                                             position,
                                             rotation,
                                             _image,
                                             _objectModel);
    // TODO: add accepted

    if (!loadAndStoreStrategy.persistObjectImagePose(&pose, false)) {
        //! if there is an error persisting the pose for any reason we should not add the pose to this manager
        return false;
    }

    //! pose has not yet been added
    poses.push_back(pose);

    createConditionalCache();

    Q_EMIT poseAdded(pose.getID());

    return true;
}

bool CachingModelManager::updateObjectImagePose(const QString &id,
                                                          QVector3D position,
                                                          QMatrix3x3 rotation) {
    Pose *pose = Q_NULLPTR;
    for (int i = 0; i < poses.size(); i++) {
        if (poses[i].getID() == id)
            pose = &poses[i];
    }

    if (!pose) {
        //! this manager does not manager the given pose
        return false;
    }

    QVector3D previousPosition = pose->getPosition();
    QMatrix3x3 previousRotation = pose->getRotation();

    pose->setPosition(position);
    pose->setRotation(rotation);

    // TODO: set accepted

    if (!loadAndStoreStrategy.persistObjectImagePose(pose, false)) {
        // if there is an error persisting the pose for any reason we should not keep the new values
        pose->setPosition(previousPosition);
        pose->setRotation(previousRotation);
        return false;
    }

    createConditionalCache();

    Q_EMIT poseUpdated(pose->getID());

    return true;
}

bool CachingModelManager::removeObjectImagePose(const QString &id) {
    Pose *pose = Q_NULLPTR;
    for (int i = 0; i < poses.size(); i++) {
        if (poses[i].getID() == id)
            pose = &poses[i];
    }

    if (!pose) {
        //! this manager does not manager the given pose
        return false;
    }

    if (!loadAndStoreStrategy.persistObjectImagePose(pose, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the pose didn't exist
        //! thus it doesn't make sense to remove the pose from this manager
        return false;
    }

    for (int i = 0; i < poses.size(); i++) {
        if (poses.at(i).getID() == id)
            poses.removeAt(i);
    }

    createConditionalCache();

    Q_EMIT poseDeleted(id);

    return true;
}

void CachingModelManager::reload() {
    images = loadAndStoreStrategy.loadImages();
    objectModels = loadAndStoreStrategy.loadObjectModels();
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT imagesChanged();
    Q_EMIT objectModelsChanged();
    Q_EMIT posesChanged();
}

void CachingModelManager::onImagesChanged() {
    images = loadAndStoreStrategy.loadImages();
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT imagesChanged();
    Q_EMIT posesChanged();
}

void CachingModelManager::onObjectModelsChanged() {
    objectModels = loadAndStoreStrategy.loadObjectModels();
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT objectModelsChanged();
    Q_EMIT posesChanged();
}

void CachingModelManager::onPosesChanged() {
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT posesChanged();
}
