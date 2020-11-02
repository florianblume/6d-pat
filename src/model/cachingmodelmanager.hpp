#ifndef CACHINGMODELMANAGER_H
#define CACHINGMODELMANAGER_H

#include "modelmanager.hpp"
#include "loadandstorestrategy.hpp"
#include <QMap>
#include <QString>
#include <QList>

/*!
 * \brief The CachingModelManager class implements the ModelManager interface. To improve the speed of the application
 * this manager chaches the list of entities and refreshes them when necessary.
 */
class CachingModelManager : public ModelManager
{

    Q_OBJECT

public:

    CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy);

    ~CachingModelManager();

    QList<ImagePtr> getImages() const override;

    QList<PosePtr> getPosesForImage(const Image &image) const override;

    QList<ObjectModelPtr> getObjectModels() const override;

    QList<PosePtr> getPosesForObjectModel(const ObjectModel &objectModel) const override;

    QList<PosePtr> getPoses() const override;

    PosePtr getPoseById(const QString &id) const override;

    QList<PosePtr> getPosesForImageAndObjectModel(
            const Image &image,
            const ObjectModel &objectModel) override;

    bool addPose(const Image &image,
                 const ObjectModel &objectModel,
                 const QVector3D &position,
                 const QMatrix3x3 &rotation) override;

    bool updatePose(const QString &id,
                    const QVector3D &position,
                    const QMatrix3x3 &rotation) override;

    bool removePose(const QString &id) override;

    void reload() override;

private:
    //! The pattern that is used to load maybe existing segmentation images
    QString segmentationImagePattern;
    //! The list of the loaded images
    QList<ImagePtr> images;
    //! Convenience map to store poses for images
    QMap<QString, QList<PosePtr>> posesForImages;
    //! The list of the loaded object models
    QList<ObjectModelPtr> objectModels;
    //! Convenience map to store poses for object models
    QMap<QString, QList<PosePtr>> posesForObjectModels;
    //! The list of the object image poses
    QList<PosePtr> poses;
    /*!
     * \brief createConditionalCache sets up the cache of poses that
     * can be retrieved for an image or for an object model.
     */
    void createConditionalCache();

private Q_SLOTS:
    void onImagesChanged();
    void onObjectModelsChanged();
    void onPosesChanged();

};

#endif // CACHINGMODELMANAGER_H
