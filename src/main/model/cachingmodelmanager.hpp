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
private:
    //! The pattern that is used to load maybe existing segmentation images
    QString segmentationImagePattern;
    //! The list of the loaded images
    QList<Image> images;
    //! Convenience map to store correspondences for images
    QMap<QString, QList<ObjectImageCorrespondence*>> correspondencesForImages;
    //! The list of the loaded object models
    QList<ObjectModel> objectModels;
    //! Convenience map to store correspondences for object models
    QMap<QString, QList<ObjectImageCorrespondence*>> correspondencesForObjectModels;
    //! The list of the object image correspondences
    QList<ObjectImageCorrespondence> correspondences;

public:
    CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy);

    ~CachingModelManager();

    void getImages(QList<const Image*> &images) const override;

    const Image* getImage(uint index) const override;

    int getImagesSize() const override;

    void getCorrespondencesForImage(const Image &image, QList<ObjectImageCorrespondence*> &correspondences) const override;

    void getObjectModels(QList<const ObjectModel*> &objectModels) const override;

    const ObjectModel* getObjectModel(uint index) const override;

    int getObjectModelsSize() const override;

    void getCorrespondencesForObjectModel(const ObjectModel &objectModel, QList<ObjectImageCorrespondence*> &correspondences) override;

    void getCorrespondences(QList<ObjectImageCorrespondence*> &correspondences) override;

    void getCorrespondencesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel, QList<ObjectImageCorrespondence*> &correspondences) override;

    bool addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

    bool updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

    bool removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) override;

    void addListener(ModelManagerListener* listener) override;

    void removeListener(ModelManagerListener* listener) override;

    void imagesChanged() override;

    void objectModelsChanged() override;

    void correspondencesChanged()  override;
};

#endif // CACHINGMODELMANAGER_H
