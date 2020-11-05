#include "cachingmodelmanager.hpp"
#include "misc/generalhelper.hpp"

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    connect(&loadAndStoreStrategy, &LoadAndStoreStrategy::dataChanged,
            this, &CachingModelManager::dataChanged);
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::createConditionalCache() {
    posesForImages.clear();
    posesForObjectModels.clear();
    for (int i = 0; i < poses.size(); i++) {
        PosePtr pose = poses[i];

        //! Setup cache of poses that can be retrieved via an image
        QVector<PosePtr> &posesForImage =
                posesForImages[pose->image()->getImagePath()];
        posesForImage.append(pose);

        //! Setup cache of poses that can be retrieved via an object model
        QVector<PosePtr> &posesForObjectModel =
                posesForObjectModels[pose->objectModel()->getPath()];
        posesForObjectModel.append(pose);
    }
}

void CachingModelManager::onDataChanged(int data) {
    Q_EMIT stateChanged(State::Loading);
    if (data == Images) {
        images = loadAndStoreStrategy.loadImages();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    if (data == ObjectModels) {
        objectModels = loadAndStoreStrategy.loadObjectModels();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    // We need to load poses no matter what
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT stateChanged(State::Ready);
    Q_EMIT dataChanged(data);
}

QVector<ImagePtr> CachingModelManager::getImages() const {
    return images;
}

QVector<PosePtr> CachingModelManager::getPosesForImage(const Image &image) const  {
    if (posesForImages.find(image.getImagePath()) != posesForImages.end()) {
        return posesForImages[image.getImagePath()];
    }

    return QVector<PosePtr>();
}

QVector<ObjectModelPtr> CachingModelManager::getObjectModels() const {
    return objectModels;
}

QVector<PosePtr> CachingModelManager::getPosesForObjectModel(const ObjectModel &objectModel) const {
    if (posesForObjectModels.find(objectModel.getPath()) != posesForObjectModels.end()) {
        return posesForObjectModels[objectModel.getPath()];
    }

    return QVector<PosePtr>();
}

QVector<PosePtr> CachingModelManager::getPoses() const {
    return poses;
}

PosePtr CachingModelManager::getPoseById(const QString &id) const {
    PosePtr result;
    auto itObj = std::find_if(
        poses.begin(), poses.end(),
        [id](PosePtr o) { return o->id() == id; }
    );
    if (itObj != poses.end()) {
        result = *itObj;
    }
    return result;
}

QVector<PosePtr> CachingModelManager::getPosesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) {
    QVector<PosePtr> posesForImageAndObjectModel;
    for (PosePtr &pose : posesForImages[image.getImagePath()]) {
        if (pose->objectModel()->getPath().compare(objectModel.getPath()) == 0) {
           posesForImageAndObjectModel.append(pose);
        }
    }
    return posesForImageAndObjectModel;
}

bool CachingModelManager::addPose(const Image &image,
                                  const ObjectModel &objectModel,
                                  const QVector3D &position,
                                  const QMatrix3x3 &rotation) {
    QVector<ImagePtr>::iterator imageIterator = find_if(images.begin(), images.end(), [image](const ImagePtr& obj) {
        return obj->getAbsoluteImagePath() == image.getAbsoluteImagePath();
    });

    if (imageIterator == images.end()) {
        return false;
    }

    QVector<ObjectModelPtr>::iterator objectModelIterator = find_if(
                objectModels.begin(), objectModels.end(), [objectModel](const ObjectModelPtr &obj) {
        return obj->getAbsolutePath() == objectModel.getAbsolutePath();
    });
    if (objectModelIterator == objectModels.end()) {
        return false;
    }

    // IMPORTANT: Use the iterator values, they return the actually managed image and object model
    // and not what the user passed (and maybe created somewhere else but with the right paths)
    ImagePtr _image = *imageIterator;
    ObjectModelPtr _objectModel = *objectModelIterator;
    PosePtr pose(new Pose(GeneralHelper::createPoseId(*_image, *_objectModel),
                                                      position,
                                                      rotation,
                                                      _image,
                                                      _objectModel));

    if (!loadAndStoreStrategy.persistPose(*pose, false)) {
        //! if there is an error persisting the pose for any reason we should not add the pose to this manager
        return false;
    }

    //! pose has not yet been added
    poses.push_back(pose);

    createConditionalCache();

    Q_EMIT poseAdded(pose);

    return true;
}

bool CachingModelManager::updatePose(const QString &id,
                                     const QVector3D &position,
                                     const QMatrix3x3 &rotation) {
    PosePtr pose;
    for (int i = 0; i < poses.size(); i++) {
        if (poses[i]->id() == id) {
            pose = poses[i];
            break;
        }
    }

    if (pose.isNull()) {
        //! this manager does not manage the given pose
        return false;
    }

    QVector3D previousPosition = pose->position();
    QMatrix3x3 previousRotation = pose->rotation().toRotationMatrix() ;

    pose->setPosition(position);
    pose->setRotation(rotation);

    if (!loadAndStoreStrategy.persistPose(*pose, false)) {
        // if there is an error persisting the pose for any reason we should not keep the new values
        pose->setPosition(previousPosition);
        pose->setRotation(previousRotation);
        return false;
    }

    createConditionalCache();

    Q_EMIT poseUpdated(pose);

    return true;
}

bool CachingModelManager::removePose(const QString &id) {
    PosePtr pose;
    for (int i = 0; i < poses.size(); i++) {
        if (poses[i]->id() == id)
            pose = poses[i];
    }

    if (!pose) {
        //! this manager does not manager the given pose
        return false;
    }

    if (!loadAndStoreStrategy.persistPose(*pose, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the pose didn't exist
        //! thus it doesn't make sense to remove the pose from this manager
        return false;
    }

    for (int i = 0; i < poses.size(); i++) {
        if (poses.at(i)->id() == id)
            poses.removeAt(i);
    }

    createConditionalCache();

    Q_EMIT poseDeleted(pose);

    return true;
}

void CachingModelManager::reload() {
    Q_EMIT stateChanged(State::Loading);
    images = loadAndStoreStrategy.loadImages();
    objectModels = loadAndStoreStrategy.loadObjectModels();
    poses = loadAndStoreStrategy.loadPoses(images, objectModels);
    createConditionalCache();
    Q_EMIT stateChanged(State::Ready);
    Q_EMIT dataChanged(Data::Images | Data::ObjectModels | Data::Poses);
}
