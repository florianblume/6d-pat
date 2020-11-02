#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "pose.hpp"
#include "image.hpp"
#include "objectmodel.hpp"
#include "settings/settingsstore.hpp"

#include <QObject>
#include <QString>
#include <QVector>
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

    LoadAndStoreStrategy(SettingsStore *settingsStore,
                         const QString &settingsIdentifier);

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

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual QVector<ImagePtr> loadImages() = 0;

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual QVector<ObjectModelPtr> loadObjectModels() = 0;

    /*!
     * \brief loadPoses Loads the poses at the given path. How the poses
     * are stored depends on the strategy.
     * \return the list of all stored poses
     */
    virtual QVector<PosePtr> loadPoses(const QVector<ImagePtr> &images,
                                       const QVector<ObjectModelPtr> &objectModels) = 0;

    void setSettingsStore(SettingsStore *value);

    void setSettingsIdentifier(const QString &value);

signals:

    void imagesChanged();
    void failedToLoadImages(const QString& message);
    void objectModelsChanged();
    void failedToLoadObjectModels(const QString &message);
    void posesChanged();
    void failedToLoadPoses(const QString &message);
    void failedToPersistPose(const QString &message);

protected slots:
    virtual void onSettingsChanged(const QString &settingsIdentifier) = 0;

protected:
    SettingsStore *settingsStore;
    QString settingsIdentifier;

};

#endif // LOADANDSTORESTRATEGY_H
