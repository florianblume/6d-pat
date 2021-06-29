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
#include <QFileSystemWatcher>

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
        None,
        ImagesPathDoesNotExist,
        SegmentationImagesPathDoesNotExist,
        NotEnoughSegmentationImages,
        CouldNotReadCamInfo,
        CamInfoDoesNotExist,
        InvalidCameraMatrices,
        CamInfoPathIsNotAJSONFile,
        NoImagesFound,
        ObjectModelsPathDoesNotExist,
        ObjectModelsPathIsNotAFolder,
        PosesPathDoesNotExist,
        PosesPathIsNotReadable,
        PosesWithInvalidPosesData,
        FailedToPersistPosePosesFileCouldNotBeRead,
        FailedToPersistPosePosesPathIsNotAFile,
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

    void setImagesPath(const QString &imagesPath);

    void setSegmentationImagesPath(const QString &path);

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual QList<ImagePtr> loadImages() = 0;

    virtual QList<QString> imagesWithInvalidCameraMatrix() const;

    void setObjectModelsPath(const QString &objectModelsPath);

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual QList<ObjectModelPtr> loadObjectModels();

    void setPosesFilePath(const QString &posesFilePath);

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses
     * are stored depends on the strategy.
     * \return the list of all stored poses
     */
    virtual QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                                       const QList<ObjectModelPtr> &objectModels) = 0;

    virtual QList<QString> posesWithInvalidPosesData() const;


Q_SIGNALS:
    void error(LoadAndStoreStrategy::Error error);
    void dataChanged(int data);

protected Q_SLOTS:
    virtual void onSettingsChanged(SettingsPtr settings);
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &filePath);

protected:
    //! Unmodifiable constants (i.e. not changable by the user at runtime)
    static const QStringList IMAGE_FILES_EXTENSIONS;
    static const QStringList OBJECT_MODEL_FILES_EXTENSIONS;

    //! Stores the path to the folder that holds the images
    QString imagesPath;
    QList<QString> m_imagesWithInvalidCameraMatrix;
    //! Stores the path to the folder that holds the object models
    QString objectModelsPath;
    //! Stores the path to the already created poses
    QString posesFilePath;
    QList<QString> m_posesWithInvalidPosesData;
    //! Stores the suffix that is used to try to load segmentation images
    QString segmentationImagesPath;

    QFileSystemWatcher watcher;

    void connectWatcherSignals();

    // We need to ignore changes to the file once after we have written
    // a new pose to it because the model manager already emits a signal
    // whenever a new pose has been added for example
    // We only want this signal when the poses file has been changed
    // externally
    bool ignorePosesFileChanged = false;

    //! Internal methods to react to path changes
    bool setPath(const QString &path, QString &oldPath);
};

//Q_DECLARE_METATYPE(LoadAndStoreStrategy::Error)

#endif // LOADANDSTORESTRATEGY_H
