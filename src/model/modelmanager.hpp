#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "pose.hpp"
#include "objectmodel.hpp"
#include "image.hpp"
#include "data.hpp"
#include "loadandstorestrategy.hpp"
#include <QObject>
#include <QString>
#include <QList>
#include <QSharedPointer>

using namespace std;

typedef QSharedPointer<LoadAndStoreStrategy> LoadAndStoreStrategyPtr;

//! Interface ModelManager defines methods to load entities of the program and store them as well.
/*!
 * A ModelManager is there to read in images and 3D models as well as poses already created by the user. It does so automatically on
 * startup and it also takes care of persisting changes constantly.
 * A ModelManager can also provide the read images, object models and poses.
 * To do so the ModelManager requires to receive a LoadAndStoreStrategy which handles the underlying details of how to persist and therefore
 * also how to load entities.
 *
 * Attention: To persist modified poses they have to be updated through the update method of the manager, otherwise the changes
 * will be lost on program restart.
*/
class ModelManager : public QObject {

    Q_OBJECT

protected:
    //! The strategy that is used to persist and also to load entities
    LoadAndStoreStrategyPtr m_loadAndStoreStrategy;

public:

    enum State {
        Loading,
        Error,
        Ready
    };

    /*!
     * \brief ModelManager Constructor of class ModelManager.
     *
     * NOTE: The constructor will not call the init method, as some things like paths have to be set first before calling the method.
     *
     * \param _LoadAndStoreStrategy
     */
    ModelManager(LoadAndStoreStrategyPtr loadAndStoreStrategy);

    virtual ~ModelManager();

    /*!
     * \brief setLoadAndStoreStrategy sets the load and store strategy used by this model manager.
     * It's purely virtual to force subclasses to take care of signals & slots de-registering.
     * \param strategy
     */
    virtual void setLoadAndStoreStrategy(LoadAndStoreStrategyPtr strategy) = 0;

    /*!
     * \brief getImages Returns the list of all images loaded by this manager.
     * \return the list of all images loaded by this manager
     */
   virtual QList<Image> images() const = 0;

    /*!
     * \brief getPosesForImage Returns all ObjectImagePoses for the image at the given path.
     * \param imagePath the path of the image
     * \return the list of poses of the image at the given path
     */
    virtual QList<Pose> posesForImage(const Image& image) const = 0;

    /*!
     * \brief getObjectModels Returns the list of all object models loaded by this manager.
     * \param objectModels the list that the object models are to be added to
     * \return the list of all objects models loaded by this manager
     */
    virtual ObjectModelList objectModels() const = 0;

    /*!
     * \brief getPosesForObjectModels Returns all ObjectImagePoses for the object model at the given path.
     * \param objectModelPath the path of the object model
     * \param poses the list that the poses are to be added to
     * \return the list of poses of the object model at the given path
     */
    virtual QList<Pose> posesForObjectModel(const ObjectModel& objectModel) const = 0;

    /*!
     * \brief getPoses Returns the poses maintained by this manager.
     * \param poses the list that the poses are to be added to
     * \return the list of poses maintained by this manager
     */
    virtual QList<Pose> poses() const = 0;

    virtual bool poseById(const QString &id, Pose *pose) const = 0;

    /*!
     * \brief getPosesForImageAndObjectModel Returns all poses for the given image and object model.
     * \param imagePath the image
     * \param objectModelPath the object model
     * \param poses the list that the poses are to be added to
     * \return all poses of the given image and given object model
     */
    virtual QList<Pose> posesForImageAndObjectModel(const Image& image,
                                                    const ObjectModel& objectModel) = 0;

    /*!
     * \brief addObjectImagePose Adds a new ObjectImagePose to the poses managed by this manager.
     * The method will return true if creating the pose was successful and persisting it as well.
     * \param objectImagePose the pose that stores all the values for the pose that will be created by
     * this manager and added to the list of managed poses
     * \return true if creating and persisting the pose was successful
     */
    virtual bool addPose(const Image &image,
                         const ObjectModel &objectModel,
                         const QVector3D &position,
                         const QMatrix3x3 &rotation) = 0;

    /*!
     * \brief addObjectImagePose Adds a new ObjectImagePose to the poses managed by this manager.
     * The method will return true if creating the pose was successful and persisting it as well.
     * \param objectImagePose the pose that stores all the values for the pose that will be created by
     * this manager and added to the list of managed poses
     * \return the new pose if it was successfully created, null otherwise
     */
    virtual bool addPose(const Pose &pose) = 0;

    /*!
     * \brief addObjectImagePose Updates the given ObjectImagePose and automatically persists it according to the
     * LoadAndStoreStrategy of this Manager. If this manager does not manage the given ObjectImageCorresopndence false will be
     * returned.
     * \param objectImagePose the pose to be updated
     * \return true if updating  and also persisting the pose was successful, false if this manager does not manage the given
     * pose or persisting it has failed
     */
    virtual bool updatePose(const QString &id,
                            const QVector3D &position,
                            const QMatrix3x3 &rotation) = 0;

    /*!
     * \brief removeObjectImagePose Removes the given ObjectImagePose if it is present in the list
     * of poses mainainted by this manager.
     * \param objectImagePose the pose to be removed
     * \return true if the pose was present and removing it, i.e. also removing it from the filesystem was
     * successful
     */
    virtual bool removePose(const QString &id) = 0;

public Q_SLOTS:
    /*!
     * \brief reload reads all data from the persitence storage again and
     * emits the corresponding signals.
     */
    virtual void reload() = 0;

Q_SIGNALS:
    void dataChanged(int data);
    void poseAdded(const Pose &pose);
    void poseUpdated(const Pose &pose);
    void poseDeleted(const Pose &pose);
    void stateChanged(ModelManager::State state, const QString &error);
};

#endif // MODELMANAGER_H
