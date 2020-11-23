#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "pose.hpp"
#include "image.hpp"
#include "objectmodel.hpp"
#include "data.hpp"
#include "settings/settingsstore.hpp"

#include <QObject>
#include <QString>
#include <QList>
#include <QDir>

using namespace std;

/*!
 * \brief The LoadAndStoreStrategy class provides the actual storing implementation that the model manager
 * uses to load entities. This way multiple implementations are possible, e.g. one writing to text files
 * or one wirting to a database.
 */
class LoadAndStoreStrategy : public QObject {

    Q_OBJECT

public:
    enum Error {
        ImagesPathDoesNotExist,
        SegmentationImagesPathDoesNotExist,
        CouldNotReadCamInfo,
        CamInfoDoesNotExist,
        CamInfoPathIsNotAJSONFile,
        NoImagesFound,
        ObjectModelsPathDoesNotExist,
        ObjectModelsPathIsNotAFolder,
        PosesPathDoesNotExist,
        FailedToPersistPosePosesFileCouldNotBeRead,
        FailedToPersistPosePosesPathIsNotAFile
    };

    LoadAndStoreStrategy();

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief persistObjectImagePose Persists the given ObjectImagePose. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImagePose the object image pose to persist
     * \param deletePose indicates whether the pose should be persistently deleted
     * \return true if persisting the object image pose was successful, false if not
     */
    virtual bool persistPose(const Pose &objectImagePose,
                             bool deletePose) = 0;

    virtual void setImagesPath(const QString &imagesPath) = 0;

    virtual void setSegmentationImagesPath(const QString &path) = 0;

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual QList<ImagePtr> loadImages() = 0;

    virtual void setObjectModelsPath(const QString &objectModelsPath) = 0;

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual QList<ObjectModelPtr> loadObjectModels() = 0;

    virtual void setPosesFilePath(const QString &posesFilePath) = 0;

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses
     * are stored depends on the strategy.
     * \return the list of all stored poses
     */
    virtual QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                                       const QList<ObjectModelPtr> &objectModels) = 0;

    void setSettingsStore(SettingsStore *value);

    void setSettingsIdentifier(const QString &value);

signals:
    void error(LoadAndStoreStrategy::Error error);
    void dataChanged(int data);

protected slots:
    virtual void onSettingsChanged(SettingsPtr settings) = 0;

};

#endif // LOADANDSTORESTRATEGY_H
