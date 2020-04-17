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

    QList<Image> getImages() const override;

    QList<Pose> getPosesForImage(const Image &image) const override;

    QList<ObjectModel> getObjectModels() const override;

    QList<Pose> getPosesForObjectModel(const ObjectModel &objectModel) override;

    QList<Pose> getPoses() override;

    QSharedPointer<Pose> getPoseById(const QString &id) override;

    QList<Pose> getPosesForImageAndObjectModel(
            const Image &image,
            const ObjectModel &objectModel) override;

    bool addObjectImagePose(Image *image,
                                      ObjectModel *objectModel,
                                      QVector3D position,
                                      QMatrix3x3 rotation) override;

    bool updateObjectImagePose(const QString &id,
                                         QVector3D position,
                                         QMatrix3x3 rotation) override;

    bool removeObjectImagePose(const QString &id) override;

    void reload() override;

private:

    //! The pattern that is used to load maybe existing segmentation images
    QString segmentationImagePattern;
    //! The list of the loaded images
    QList<Image> images;
    //! Convenience map to store poses for images
    QMap<QString, QList<Pose>> posesForImages;
    //! The list of the loaded object models
    QList<ObjectModel> objectModels;
    //! Convenience map to store poses for object models
    QMap<QString, QList<Pose>> posesForObjectModels;
    //! The list of the object image poses
    QList<Pose> poses;
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
