#include "cachingmodelmanager.hpp"
#include "misc/generalhelper.hpp"

#include <QApplication>

CachingModelManager::CachingModelManager(LoadAndStoreStrategyPtr loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    connect(loadAndStoreStrategy.get(), &LoadAndStoreStrategy::dataChanged,
            this, &CachingModelManager::dataChanged);
    connect(loadAndStoreStrategy.get(), &LoadAndStoreStrategy::error,
            this, &CachingModelManager::onLoadAndStoreStrategyError);
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::createConditionalCache() {
    m_posesForImages.clear();
    m_posesForObjectModels.clear();
    for (int i = 0; i < m_poses.size(); i++) {
        PosePtr pose = m_poses[i];

        //! Setup cache of poses that can be retrieved via an image
        QList<PosePtr> &posesForImage =
                m_posesForImages[pose->image()->imagePath()];
        posesForImage.append(pose);

        //! Setup cache of poses that can be retrieved via an object model
        QList<PosePtr> &posesForObjectModel =
                m_posesForObjectModels[pose->objectModel()->path()];
        posesForObjectModel.append(pose);
    }
}

void CachingModelManager::onDataChanged(int data) {
    Q_EMIT stateChanged(State::Loading, LoadAndStoreStrategy::Error::None);
    if (data == Images) {
        m_images = m_loadAndStoreStrategy->loadImages();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    if (data == ObjectModels) {
        m_objectModels = m_loadAndStoreStrategy->loadObjectModels();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    // We need to load poses no matter what
    m_poses = m_loadAndStoreStrategy->loadPoses(m_images, m_objectModels);
    createConditionalCache();
    Q_EMIT stateChanged(ModelManager::State::Ready, LoadAndStoreStrategy::Error::None);
    Q_EMIT dataChanged(data);
}

QList<ImagePtr> CachingModelManager::images() const {
    return m_images;
}

QList<PosePtr> CachingModelManager::posesForImage(const Image &image) const  {
    if (m_posesForImages.find(image.imagePath()) != m_posesForImages.end()) {
        return m_posesForImages[image.imagePath()];
    }

    return QList<PosePtr>();
}

QList<ObjectModelPtr> CachingModelManager::objectModels() const {
    return m_objectModels;
}

QList<PosePtr> CachingModelManager::posesForObjectModel(const ObjectModel &objectModel) const {
    if (m_posesForObjectModels.find(objectModel.path()) != m_posesForObjectModels.end()) {
        return m_posesForObjectModels[objectModel.path()];
    }

    return QList<PosePtr>();
}

QList<PosePtr> CachingModelManager::poses() const {
    return m_poses;
}

PosePtr CachingModelManager::poseById(const QString &id) const {
    PosePtr result;
    auto itObj = std::find_if(
        m_poses.begin(), m_poses.end(),
        [id](PosePtr o) { return o->id() == id; }
    );
    if (itObj != m_poses.end()) {
        result = *itObj;
    }
    return result;
}

QList<PosePtr> CachingModelManager::posesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) {
    QList<PosePtr> posesForImageAndObjectModel;
    for (PosePtr &pose : m_posesForImages[image.imagePath()]) {
        if (pose->objectModel()->path().compare(objectModel.path()) == 0) {
           posesForImageAndObjectModel.append(pose);
        }
    }
    return posesForImageAndObjectModel;
}

PosePtr CachingModelManager::addPose(ImagePtr image,
                                     ObjectModelPtr objectModel,
                                     const QVector3D &position,
                                     const QMatrix3x3 &rotation) {
    Q_ASSERT(image);
    Q_ASSERT(objectModel);
    return this->addPose(Pose(GeneralHelper::createPoseId(*image, *objectModel),
                              position,
                              rotation,
                              image,
                              objectModel));
}

PosePtr CachingModelManager::addPose(const Pose &pose) {
    // Persist the pose
    if (!m_loadAndStoreStrategy->persistPose(pose, false)) {
        //! if there is an error persisting the pose for any reason we should not add the pose to this manager
        return PosePtr();
    }

    //! pose has not yet been added
    PosePtr newPose(new Pose(pose));
    m_poses.push_back(newPose);

    createConditionalCache();

    Q_EMIT poseAdded(newPose);

    return newPose;
}

bool CachingModelManager::updatePose(const QString &id,
                                     const QVector3D &position,
                                     const QMatrix3x3 &rotation) {
    PosePtr pose;
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i]->id() == id) {
            pose = m_poses[i];
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

    if (!m_loadAndStoreStrategy->persistPose(*pose, false)) {
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
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i]->id() == id)
            pose = m_poses[i];
    }

    if (!pose) {
        //! this manager does not manager the given pose
        return false;
    }

    if (!m_loadAndStoreStrategy->persistPose(*pose, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the pose didn't exist
        //! thus it doesn't make sense to remove the pose from this manager
        return false;
    }

    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses.at(i)->id() == id)
            m_poses.removeAt(i);
    }

    createConditionalCache();

    Q_EMIT poseDeleted(pose);

    return true;
}

void CachingModelManager::reload() {
    Q_EMIT stateChanged(CachingModelManager::State::Loading, LoadAndStoreStrategy::Error::None);
    m_images = m_loadAndStoreStrategy->loadImages();
    m_objectModels = m_loadAndStoreStrategy->loadObjectModels();
    m_poses = m_loadAndStoreStrategy->loadPoses(m_images, m_objectModels);
    createConditionalCache();
    Q_EMIT dataReady();
}

void CachingModelManager::onLoadAndStoreStrategyError(LoadAndStoreStrategy::Error error) {
    Q_EMIT stateChanged(CachingModelManager::State::ErrorOccured, error);
}

void CachingModelManager::dataReady() {
    Q_EMIT stateChanged(CachingModelManager::State::Ready, LoadAndStoreStrategy::Error::None);
    Q_EMIT dataChanged(Data::Images | Data::ObjectModels | Data::Poses);
}
