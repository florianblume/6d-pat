#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "objectimagecorrespondence.h"
#include "image.h"
#include "loadandstorestrategy.h"
#include <string>
#include <list>
#include <map>

using namespace std;

//! Class ModelManager manages data throughout the program and takes care of persistency.
/*!
 * A ModelManager is there to read in images and 3D models as well as correspondences already created by the user. It does so automatically on
 * startup and it also takes care of persisting changes constantly.
 * A ModelManager can also provide the read images, object models and correspondences.
 *
 * Attention: To persist modified correspondences they have to be updated through the update method of the manager, otherwise the changes
 * will be lost on program restart.
*/
class ModelManager
{
private:
    //! Stores the path to the folder that holds the images
    string imagesPath;
    //! The pattern that is used to load maybe existing segmentation images
    string segmentationImagePattern;
    //! Stores the path to the folder that holds the object models
    string objectModelsPath;
    //! Stores the path to the already created correspondences
    string correspondencesPath;
    //! The list of the loaded images
    list<Image> images;
    //! Convenience map to store correspondences for images
    map<string, list<ObjectImageCorrespondence*>> correspondencesForImages;
    //! The list of the loaded object models
    list<ObjectModel> objectModels;
    //! Convenience map to store correspondences for object models
    map<string, list<ObjectImageCorrespondence*>> correspondencesForObjectModels;
    //! The list of the object image correspondences
    list<ObjectImageCorrespondence> correspondences;
    //! The strategy that is used to persist and also to load entities
    LoadAndStoreStrategy loadAndStoreStrategy;

    /*!
     * \brief writeSettings Persists settings like paths.
     */
    void writeSettings();

    /*!
     * \brief readSettings Reads settings like paths.
     */
    void readSettings();

    /*!
     * \brief load Loads all entities at the given paths.
     */
    void load();

public:

    ModelManager();

    ~ModelManager();

    /*!
     * \brief init Initializes this ModelManager. The init method takes care of reading the last set paths from the settings and
     * automatically loads the entities at those locations.
     */
    void init();

    /*!
     * \brief setImagesPath Sets the path to the folder where the images that are to be annotated are located. After setting the
     * path the manager will automatically try to load the images and also their respective segmentation using the pattern for
     * segmentation image names.
     * \param path the path to the folder where the images are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setImagesPath(string path);

    /*!
     * \brief getImagesPath Returns the path that this manager uses to load images.
     * \return the path that this manager uses to load images
     */
    string getImagesPath() const;

    /*!
     * \brief setObjectModelsPath Sets the path to the folder where the object models are located. After setting the path the
     * manager will automatically try to load the objects.
     * \param path the path to the folder where the objects are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setObjectModelsPath(string path);

    /*!
     * \brief getObjectModelsPath Returns the path that this manager uses to load object models.
     * \return the path that this manager uses to load object models
     */
    string getObjectModelsPath() const;

    /*!
     * \brief setCorrespondencesPath Sets the path to the folder where object image correspondences are stored at. After setting
     * the path the manager will automatically try to load the already existing correspondences and will store all future
     * correspondences at this location.
     * \param path the path where the manager should store correspondences at or where some correspondences already exist that
     * are meant to be loaded
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setCorrespondencesPath(string path);

    /*!
     * \brief getCorrespondencesPath Returns the path that this manager uses to store and load correspondences.
     * \return the path that this manager uses to store and load correspondences
     */
    string getCorrespondencesPath() const;

    /*!
     * \brief setSegmentationImagePattern Sets the pattern that is used to simoultaneously load segmented images. E.g. if an
     * image is named image1.png and the segmented image is named image1_segmented.png then the pattern should be %name%_segmented.png.
     * \param pattern the pattern that is used to load segmented images
     */
    void setSegmentationImagePattern(string pattern);

    /*!
     * \brief getSegmentationImagePattern Returns the pattern that is used to load segmentation images.
     * \return the pattern that this manager uses to load segmentation images
     */
    string getSegmentationImagePattern();

    /*!
     * \brief getImages Returns the list of all images loaded by this manager.
     * \return the list of all images loaded by this manager
     */
    const list<Image>* getImages() const;

    /*!
     * \brief getCorrespondencesForImage Returns all ObjectImageCorrespondences for the image at the given path.
     * \param imagePath the path of the image
     * \return the list of correspondences of the image at the given path
     */
    list<ObjectImageCorrespondence*> getCorrespondencesForImage(string imagePath) const;

    /*!
     * \brief getObjectModels Returns the list of all object models loaded by this manager.
     * \return the list of all objects models loaded by this manager
     */
    const list<ObjectModel>* getObjectModels() const;

    /*!
     * \brief getCorrespondencesForObjectModels Returns all ObjectImageCorrespondences for the object model at the given path.
     * \param objectModelPath the path of the object model
     * \return the list of correspondences of the object model at the given path
     */
    const list<ObjectImageCorrespondence*> getCorrespondencesForObjectModels(string objectModelPath) const;

    /*!
     * \brief getCorrespondences Returns the correspondences maintained by this manager.
     * \return the list of correspondences maintained by this manager
     */
    const list<ObjectImageCorrespondence>* getCorrespondences() const;

    /*!
     * \brief getCorrespondencesForImageAndObjectModel Returns all correspondences for the given image and object model.
     * \param imagePath the image
     * \param objectModelPath the object model
     * \return all correspondences of the given image and given object model
     */
    const list<ObjectImageCorrespondence*> getCorrespondencesForImageAndObjectModel(string imagePath, string objectModelPath);

    /*!
     * \brief addObjectImageCorrespondence Adds the given ObjectImageCorrespondence to the list of correspondences
     * maintained by this manager, if the correspondencen is not present yet. If it is the values of the correspondence
     * will be updated. After adding or updating the correspondence the manager automatically persists the correspondence.
     * \param objectImageCorrespondence the correspondence to be added
     * \return true if adding or updating  and also persisting the correspondence was successful, false if e.g. the filesystem
     * could not persist the correspondence
     */
    bool addOrUpdateObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence);

    /*!
     * \brief removeObjectImageCorrespondence Removes the given ObjectImageCorrespondence if it is present in the list
     * of correspondences mainainted by this manager.
     * \param objectImageCorrespondence the correspondence to be removed
     * \return true if the correspondence was present and removing it, i.e. also removing it from the filesystem was
     * successful
     */
    bool removeObjectImageCorrespondence(ObjectImageCorrespondence& objectImageCorrespondence);

};

#endif // MODELMANAGER_H
