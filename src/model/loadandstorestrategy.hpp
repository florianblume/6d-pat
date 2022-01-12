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

    LoadAndStoreStrategy();

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief useSettings uses the given settings to adjust all attributes
     * of this LoadAndStoreStrategy. Can be overwritten by subclasses to
     * care for special attributes.
     * \param settings the settings to use
     */
    virtual void applySettings(SettingsPtr settings);

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
    virtual QList<Image> loadImages() = 0;

    virtual QList<QString> imagesWithInvalidData() const;

    void setObjectModelsPath(const QString &objectModelsPath);

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual ObjectModelList loadObjectModels() = 0;

    void setPosesFilePath(const QString &posesFilePath);

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses
     * are stored depends on the strategy.
     * \return the list of all stored poses
     */
    virtual QList<Pose> loadPoses(const QList<Image> &images,
                                  const ObjectModelList &objectModels) = 0;

    virtual QList<QString> posesWithInvalidData() const;


Q_SIGNALS:
    void error(const QString &error);
    void dataChanged(int data);

protected Q_SLOTS:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &filePath);

protected:
    void connectWatcherSignals();

    //! Internal methods to react to path changes
    bool setPath(const QString &path, QString &oldPath);

protected:
    //! Unmodifiable constants (i.e. not changable by the user at runtime)
    static const QStringList IMAGE_FILES_EXTENSIONS;
    static const QStringList OBJECT_MODEL_FILES_EXTENSIONS;

    //! Stores the path to the folder that holds the images
    QString m_imagesPath;
    QList<QString> m_imagesWithInvalidData;
    //! Stores the path to the folder that holds the object models
    QString m_objectModelsPath;
    QList<QString> m_objectModelsWithInvalidData;
    //! Stores the path to the already created poses
    QString m_posesFilePath;
    QList<QString> m_posesWithInvalidData;
    //! Stores the suffix that is used to try to load segmentation images
    QString m_segmentationImagesPath;

    QFileSystemWatcher m_fileSystemWatcher;

    // We need to ignore changes to the file once after we have written
    // a new pose to it because the model manager already emits a signal
    // whenever a new pose has been added for example
    // We only want this signal when the poses file has been changed
    // externally
    bool m_ignorePosesFileChanged = false;
};

//Q_DECLARE_METATYPE(LoadAndStoreStrategy::Error)

#endif // LOADANDSTORESTRATEGY_H
