#ifndef TEXTFILELOADANDSTORESTRATEGY_H
#define TEXTFILELOADANDSTORESTRATEGY_H

#include "loadandstorestrategy.hpp"
#include <QString>
#include <QStringList>
#include <QList>
#include <QFileSystemWatcher>

/*!
 * \brief The TextFileLoadAndStoreStrategy class is a simple implementation of a LoadAndStoreStrategy that makes no use of
 * caching already loaded images or object models and writes poses to text files. That is why it is important to set the
 * proper path to the folder of poses before using this strategy.
 */
class JsonLoadAndStoreStrategy : public LoadAndStoreStrategy
{

    Q_OBJECT

public:
    //! Unmodifiable constants (i.e. not changable by the user at runtime)
    static const QStringList IMAGE_FILES_EXTENSIONS;
    static const QStringList OBJECT_MODEL_FILES_EXTENSIONS;

public:
    /*!
     * \brief TextFileLoadAndStoreStrategy Constructor of this strategy. The paths MUST be set aferwards to use it
     * properly, otherwise the strategy won't deliver any content.
     */
    JsonLoadAndStoreStrategy();

    ~JsonLoadAndStoreStrategy();

    bool persistPose(const Pose &pose, bool deletePose) override;

    void setImagesPath(const QString &imagesPath) override;

    void setSegmentationImagesPath(const QString &path) override;

    QList<ImagePtr> loadImages() override;

    QList<QString> imagesWithInvalidCameraMatrix() const override;

    void setObjectModelsPath(const QString &objectModelsPath) override;

    QList<ObjectModelPtr> loadObjectModels() override;

    void setPosesFilePath(const QString &posesFilePath) override;

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses are stored depends on the
     * strategy.
     *
     * IMPORTANT: This implementation of LoadAndStoreStrategy makes use of text files to store poses, this means that the
     * path to the folder has to be set before this method is called. Failing to do so will raise an exception.
     *
     * \param images the images to insert as references into the respective poses
     * \param objectModels the object models to insert as reference into the respective pose
     * \param poses the list that the corresondences are to be added to
     * \return the list of all stored poses
     * \throws an exception if the path to the folder that should hold the poses has not been set previously
     */
    QList<PosePtr> loadPoses(const QList<ImagePtr> &images,
                               const QList<ObjectModelPtr> &objectModels) override;

    QList<QString> posesWithInvalidPosesData() const override;

protected slots:
    void onSettingsChanged(SettingsPtr settings) override;

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &filePath);

private:
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

#endif // TEXTFILELOADANDSTORESTRATEGY_H
