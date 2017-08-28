#ifndef CACHINGMODELMANAGER_H
#define CACHINGMODELMANAGER_H

#include "modelmanager.hpp"
#include "loadandstorestrategy.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <map>
#include <string>
#include <vector>

/*!
 * \brief The CachingModelManager class implements the ModelManager interface. To improve the speed of the application
 * this manager chaches the list of entities and refreshes them when necessary.
 */
class CachingModelManager : public ModelManager
{
private:
    //! The pattern that is used to load maybe existing segmentation images
    string segmentationImagePattern;
    //! The list of the loaded images
    vector<Image> images;
    //! Convenience map to store correspondences for images
    map<string, vector<ObjectImageCorrespondence*>> correspondencesForImages;
    //! The list of the loaded object models
    vector<ObjectModel> objectModels;
    //! Convenience map to store correspondences for object models
    map<string, vector<ObjectImageCorrespondence*>> correspondencesForObjectModels;
    //! The list of the object image correspondences
    vector<ObjectImageCorrespondence> correspondences;

public:
    CachingModelManager(LoadAndStoreStrategy& _loadAndStoreStrategy);

    ~CachingModelManager();

    /*!
     * \brief getImages Returns the list of all images loaded by this manager.
     * \return the list of all images loaded by this manager
     */
    vector<Image>* getImages();

    /*!
     * \brief getCorrespondencesForImage Returns all ObjectImageCorrespondences for the image at the given path.
     * \param imagePath the path of the image
     * \return the list of correspondences of the image at the given path
     */
    vector<ObjectImageCorrespondence*> getCorrespondencesForImage(string imagePath);

    /*!
     * \brief getObjectModels Returns the list of all object models loaded by this manager.
     * \return the list of all objects models loaded by this manager
     */
    vector<ObjectModel>* getObjectModels();

    /*!
     * \brief getCorrespondencesForObjectModels Returns all ObjectImageCorrespondences for the object model at the given path.
     * \param objectModelPath the path of the object model
     * \return the list of correspondences of the object model at the given path
     */
    vector<ObjectImageCorrespondence*> getCorrespondencesForObjectModel(string objectModelPath);

    /*!
     * \brief getCorrespondences Returns the correspondences maintained by this manager.
     * \return the list of correspondences maintained by this manager
     */
    vector<ObjectImageCorrespondence>* getCorrespondences();

    /*!
     * \brief getCorrespondencesForImageAndObjectModel Returns all correspondences for the given image and object model.
     * \param imagePath the image
     * \param objectModelPath the object model
     * \return all correspondences of the given image and given object model
     */
    vector<ObjectImageCorrespondence*> getCorrespondencesForImageAndObjectModel(string imagePath, string objectModelPath);

    /*!
     * \brief addObjectImageCorrespondence Adds a new ObjectImageCorrespondence to the correspondences managed by this manager.
     * The method will return true if creating the correspondence was successful and persisting it as well.
     * \param objectImageCorrespondence the correspondence that stores all the values for the correspondence that will be created by
     * this manager and added to the list of managed correspondences
     * \return true if creating and persisting the correspondence was successful
     */
    bool addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence);

    /*!
     * \brief addObjectImageCorrespondence Updates the given ObjectImageCorrespondence and automatically persists it according to the
     * LoadAndStoreStrategy of this Manager. If this manager does not manage the given ObjectImageCorresopndence false will be
     * returned.
     * \param objectImageCorrespondence the correspondence to be updated
     * \return true if updating  and also persisting the correspondence was successful, false if this manager does not manage the given
     * correspondence or persisting it has failed
     */
    bool updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence);

    /*!
     * \brief removeObjectImageCorrespondence Removes the given ObjectImageCorrespondence if it is present in the vector
     * of correspondences mainainted by this manager.
     * \param objectImageCorrespondence the correspondence to be removed
     * \return true if the correspondence was present and removing it, i.e. also removing it from the filesystem was
     * successful
     */
    bool removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence);


    /*!
     * \brief addListener Adds a listener to this manager. The listener will be notified if e.g. the images change,
     * or correspondences or models.
     * \param listener the listener to add
     */
    void addListener(ModelManagerListener* listener);

    /*!
     * \brief removeListener Removes the given listener from the list of listeners of this model manager.
     * \param listener the listener to remove
     */
    void removeListener(ModelManagerListener* listener);

    void imagesChanged();

    void objectModelsChanged();

    void corresopndencesChanged() ;
};

#endif // CACHINGMODELMANAGER_H
