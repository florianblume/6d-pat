#include "cachingmodelmanager.hpp"
#include "misc/generalhelper.hpp"

#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QApplication>

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    connect(&loadAndStoreStrategy, &LoadAndStoreStrategy::dataChanged,
            this, &CachingModelManager::dataChanged);
    connect(&reloadFutureWatcher, &QFutureWatcher<void>::finished, this, &CachingModelManager::dataReady);
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
    Q_EMIT stateChanged(State::Loading);
    if (data == Images) {
        m_images = loadAndStoreStrategy.loadImages();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    if (data == ObjectModels) {
        m_objectModels = loadAndStoreStrategy.loadObjectModels();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    // We need to load poses no matter what
    m_poses = loadAndStoreStrategy.loadPoses(m_images, m_objectModels);
    createConditionalCache();
    Q_EMIT stateChanged(State::Ready);
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

bool CachingModelManager::addPose(const Image &image,
                                  const ObjectModel &objectModel,
                                  const QVector3D &position,
                                  const QMatrix3x3 &rotation) {
    QList<ImagePtr>::iterator imageIterator = find_if(m_images.begin(), m_images.end(), [image](const ImagePtr& obj) {
        return obj->absoluteImagePath() == image.absoluteImagePath();
    });

    if (imageIterator == m_images.end()) {
        return false;
    }

    QList<ObjectModelPtr>::iterator objectModelIterator = find_if(
                m_objectModels.begin(), m_objectModels.end(), [objectModel](const ObjectModelPtr &obj) {
        return obj->absolutePath() == objectModel.absolutePath();
    });
    if (objectModelIterator == m_objectModels.end()) {
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
    m_poses.push_back(pose);

    createConditionalCache();

    Q_EMIT poseAdded(pose);

    return true;
}

bool CachingModelManager::addPose(const Pose &pose) {
    return this->addPose(*pose.image(),
                         *pose.objectModel(),
                         pose.position(),
                         pose.rotation().toRotationMatrix());
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
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i]->id() == id)
            pose = m_poses[i];
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

    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses.at(i)->id() == id)
            m_poses.removeAt(i);
    }

    createConditionalCache();

    Q_EMIT poseDeleted(pose);

    return true;
}

void CachingModelManager::reload() {
    Q_EMIT stateChanged(State::Loading);
    reloadFuture = QtConcurrent::run(this, &CachingModelManager::threaddedReload);
    reloadFutureWatcher.setFuture(reloadFuture);
}

void CachingModelManager::threaddedReload() {
    m_images = loadAndStoreStrategy.loadImages();
    m_objectModels = loadAndStoreStrategy.loadObjectModels();
    m_poses = loadAndStoreStrategy.loadPoses(m_images, m_objectModels);
    createConditionalCache();
}

void CachingModelManager::dataReady() {
    Q_EMIT stateChanged(State::Ready);
    Q_EMIT dataChanged(Data::Images | Data::ObjectModels | Data::Poses);
}
