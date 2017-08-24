#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "objectimagecorrespondence.hpp"
#include "image.hpp"
#include "loadandstorestrategy.hpp"
#include "loadandstorestrategylistener.hpp"
#include <string>
#include <vector>
#include <map>

using namespace std;

//! Interface ModelManager defines methods to load entities of the program and store them as well.
/*!
 * A ModelManager is there to read in images and 3D models as well as correspondences already created by the user. It does so automatically on
 * startup and it also takes care of persisting changes constantly.
 * A ModelManager can also provide the read images, object models and correspondences.
 * To do so the ModelManager requires to receive a LoadAndStoreStrategy which handles the underlying details of how to persist and therefore
 * also how to load entities.
 *
 * Attention: To persist modified correspondences they have to be updated through the update method of the manager, otherwise the changes
 * will be lost on program restart.
*/
class ModelManager : LoadAndStoreStrategyListener
{

protected:
    //! The strategy that is used to persist and also to load entities
    LoadAndStoreStrategy* loadAndStoreStrategy;

public:

    /*!
     * \brief ModelManager Constructor of class ModelManager.
     *
     * NOTE: The constructor will not call the init method, as some things like paths have to be set first before calling the method.
     *
     * \param _LoadAndStoreStrategy
     */
    ModelManager(LoadAndStoreStrategy& _LoadAndStoreStrategy);

    virtual ~ModelManager();

    /*!
     * \brief getImages Returns the list of all images loaded by this manager.
     * \return the list of all images loaded by this manager
     */
   virtual  vector<Image>* getImages() = 0;

    /*!
     * \brief getCorrespondencesForImage Returns all ObjectImageCorrespondences for the image at the given path.
     * \param imagePath the path of the image
     * \return the list of correspondences of the image at the given path
     */
    virtual vector<ObjectImageCorrespondence*> getCorrespondencesForImage(string imagePath) = 0;

    /*!
     * \brief getObjectModels Returns the list of all object models loaded by this manager.
     * \return the list of all objects models loaded by this manager
     */
    virtual vector<ObjectModel>* getObjectModels() = 0;

    /*!
     * \brief getCorrespondencesForObjectModels Returns all ObjectImageCorrespondences for the object model at the given path.
     * \param objectModelPath the path of the object model
     * \return the list of correspondences of the object model at the given path
     */
    virtual vector<ObjectImageCorrespondence*> getCorrespondencesForObjectModel(string objectModelPath) = 0;

    /*!
     * \brief getCorrespondences Returns the correspondences maintained by this manager.
     * \return the list of correspondences maintained by this manager
     */
    virtual vector<ObjectImageCorrespondence>* getCorrespondences() = 0;

    /*!
     * \brief getCorrespondencesForImageAndObjectModel Returns all correspondences for the given image and object model.
     * \param imagePath the image
     * \param objectModelPath the object model
     * \return all correspondences of the given image and given object model
     */
    virtual vector<ObjectImageCorrespondence*> getCorrespondencesForImageAndObjectModel(string imagePath, string objectModelPath) = 0;

    /*!
     * \brief addObjectImageCorrespondence Adds a new ObjectImageCorrespondence to the correspondences managed by this manager.
     * The method will return true if creating the correspondence was successful and persisting it as well.
     * \param objectImageCorrespondence the correspondence that stores all the values for the correspondence that will be created by
     * this manager and added to the list of managed correspondences
     * \return true if creating and persisting the correspondence was successful
     */
    virtual bool addObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorespondence) = 0;

    /*!
     * \brief addObjectImageCorrespondence Updates the given ObjectImageCorrespondence and automatically persists it according to the
     * LoadAndStoreStrategy of this Manager. If this manager does not manage the given ObjectImageCorresopndence false will be
     * returned.
     * \param objectImageCorrespondence the correspondence to be updated
     * \return true if updating  and also persisting the correspondence was successful, false if this manager does not manage the given
     * correspondence or persisting it has failed
     */
    virtual bool updateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) = 0;

    /*!
     * \brief removeObjectImageCorrespondence Removes the given ObjectImageCorrespondence if it is present in the list
     * of correspondences mainainted by this manager.
     * \param objectImageCorrespondence the correspondence to be removed
     * \return true if the correspondence was present and removing it, i.e. also removing it from the filesystem was
     * successful
     */
    virtual bool removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence) = 0;

    /**
      ######################################
      Interface LoadAndStoreStrategyListener
      ######################################
    */

    virtual void imagesChanged() = 0;

    virtual void objectModelsChanged() = 0;

    virtual void corresopndencesChanged() = 0;

};

#endif // MODELMANAGER_H
