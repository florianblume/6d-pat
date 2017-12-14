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

public:

    CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy);

    ~CachingModelManager();

    QList<Image> getImages() const override;

    QList<ObjectImageCorrespondence> getCorrespondencesForImage(const Image &image) const override;

    QList<ObjectModel> getObjectModels() const override;

    QList<ObjectImageCorrespondence> getCorrespondencesForObjectModel(const ObjectModel &objectModel) override;

    QList<ObjectImageCorrespondence> getCorrespondences() override;

    QList<ObjectImageCorrespondence> getCorrespondencesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) override;

    bool addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

    bool updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

    bool removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

private:

    //! The pattern that is used to load maybe existing segmentation images
    QString segmentationImagePattern;
    //! The list of the loaded images
    QList<Image> images;
    //! Convenience map to store correspondences for images
    QMap<QString, QList<ObjectImageCorrespondence>> correspondencesForImages;
    //! The list of the loaded object models
    QList<ObjectModel> objectModels;
    //! Convenience map to store correspondences for object models
    QMap<QString, QList<ObjectImageCorrespondence>> correspondencesForObjectModels;
    //! The list of the object image correspondences
    QList<ObjectImageCorrespondence> correspondences;
    /*!
     * \brief createConditionalCache sets up the cache of correspondences that
     * can be retrieved for an image or for an object model.
     */
    void createConditionalCache();

private slots:

    void onImagesChanged();
    void onObjectModelsChanged();
    void onCorrespondencesChanged();

};

#endif // CACHINGMODELMANAGER_H
