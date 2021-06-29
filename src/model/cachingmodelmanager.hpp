#ifndef CACHINGMODELMANAGER_H
#define CACHINGMODELMANAGER_H

#include "modelmanager.hpp"
#include "loadandstorestrategy.hpp"
#include <QMap>
#include <QString>
#include <QList>
#include <QFuture>
#include <QFutureWatcher>

/*!
 * \brief The CachingModelManager class implements the ModelManager interface. To improve the speed of the application
 * this manager chaches the list of entities and refreshes them when necessary.
 */
class CachingModelManager : public ModelManager
{

    Q_OBJECT

public:

    CachingModelManager(LoadAndStoreStrategyPtr loadAndStoreStrategy);

    ~CachingModelManager();

    QList<ImagePtr> images() const override;

    QList<PosePtr> posesForImage(const Image &image) const override;

    QList<ObjectModelPtr> objectModels() const override;

    QList<PosePtr> posesForObjectModel(const ObjectModel &objectModel) const override;

    QList<PosePtr> poses() const override;

    PosePtr poseById(const QString &id) const override;

    QList<PosePtr> posesForImageAndObjectModel(const Image &image,
                                               const ObjectModel &objectModel) override;

    PosePtr addPose(ImagePtr image,
                    ObjectModelPtr objectModel,
                    const QVector3D &position,
                    const QMatrix3x3 &rotation) override;

    PosePtr addPose(const Pose &pose) override;

    bool updatePose(const QString &id,
                    const QVector3D &position,
                    const QMatrix3x3 &rotation) override;

    bool removePose(const QString &id) override;

public Q_SLOTS:
    void reload() override;

private Q_SLOTS:
    // Callback for threadded data loading
    void dataReady();
    void onDataChanged(int data);
    void onLoadAndStoreStrategyError(LoadAndStoreStrategy::Error error);

private:
    /*!
     * \brief createConditionalCache sets up the cache of poses that
     * can be retrieved for an image or for an object model.
     */
    void createConditionalCache();

private:
    //! The pattern that is used to load maybe existing segmentation images
    QString m_segmentationImagePattern;
    //! The list of the loaded images
    QList<ImagePtr> m_images;
    //! Convenience map to store poses for images
    QMap<QString, QList<PosePtr>> m_posesForImages;
    //! The list of the loaded object models
    QList<ObjectModelPtr> m_objectModels;
    //! Convenience map to store poses for object models
    QMap<QString, QList<PosePtr>> m_posesForObjectModels;
    //! The list of the object image poses
    QList<PosePtr> m_poses;

};

#endif // CACHINGMODELMANAGER_H
