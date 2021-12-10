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

    void setLoadAndStoreStrategy(LoadAndStoreStrategyPtr strategy) override;

    QList<Image> images() const override;

    QList<Pose> posesForImage(const Image &image) const override;

    QList<ObjectModel> objectModels() const override;

    QList<Pose> posesForObjectModel(const ObjectModel &objectModel) const override;

    QList<Pose> poses() const override;

    bool poseById(const QString &id, Pose *pose) const override;

    QList<Pose> posesForImageAndObjectModel(const Image &image,
                                            const ObjectModel &objectModel) override;

    bool addPose(const Image &image,
                 const ObjectModel &objectModel,
                 const QVector3D &position,
                 const QMatrix3x3 &rotation) override;

    bool addPose(const Pose &pose) override;

    bool updatePose(const QString &id,
                    const QVector3D &position,
                    const QMatrix3x3 &rotation) override;

    bool removePose(const QString &id) override;

Q_SIGNALS:
    void dataReady();

public Q_SLOTS:
    void reload() override;

private Q_SLOTS:
    // Callback for threadded data loading
    void onDataChanged(int data);
    void onLoadAndStoreStrategyError(const QString &error);

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
    QList<Image> m_images;
    //! Convenience map to store poses for images
    QMap<QString, QList<Pose>> m_posesForImages;
    //! The list of the loaded object models
    QList<ObjectModel> m_objectModels;
    //! Convenience map to store poses for object models
    QMap<QString, QList<Pose>> m_posesForObjectModels;
    //! The list of the object image poses
    QList<Pose> m_poses;

};

#endif // CACHINGMODELMANAGER_H
