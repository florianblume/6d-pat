#ifndef TEXTFILELOADANDSTORESTRATEGY_H
#define TEXTFILELOADANDSTORESTRATEGY_H

#include "loadandstorestrategy.hpp"
#include <map>

/*!
 * \brief The TextFileLoadAndStoreStrategy class is a simple implementation of a LoadAndStoreStrategy that makes no use of
 * caching already loaded images or object models and writes correspondences to text files. That is why it is important to set the
 * proper path to the folder of correspondences before using this strategy.
 */
class TextFileLoadAndStoreStrategy : public LoadAndStoreStrategy
{
private:
    //! Stores the path to the folder that holds the images
    path imagesPath;
    //! Stores the path to the folder that holds the object models
    path objectModelsPath;
    //! Stores the path to the already created correspondences
    path correspondencesPath;
    //! Stores the suffix that is used to try to load segmentation images
    string segmentationImageFilesSuffix = "_GT";
    //! Stores the extension of the image files that are to be loaded
    string imageFilesExtension = ".png";

public:
    /*!
     * \brief TextFileLoadAndStoreStrategy Constructor of this strategy. The paths MUST be set aferwards to use it
     * properly, otherwise the strategy won't deliver any content.
     */
    TextFileLoadAndStoreStrategy();

    /*!
     * \brief TextFileLoadAndStoreStrategy Convenience constructor setting the paths already.
     * \param _imagesPath
     * \param _objectModelsPath
     * \param _correspondencesPath
     */
    TextFileLoadAndStoreStrategy(const path _imagesPath, const path _objectModelsPath, const path _correspondencesPath);

    ~TextFileLoadAndStoreStrategy();

    bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) override;

    vector<Image> loadImages() override;

    vector<ObjectModel> loadObjectModels() override;

    /*!
     * \brief loadCorrespondences Loads the correspondences at the given path. How the correspondences are stored depends on the
     * strategy.
     *
     * IMPORTANT: This implementation of LoadAndStoreStrategy makes use of text files to store correspondences, this means that the
     * path to the folder has to be set before this method is called. Failing to do so will raise an exception.
     *
     * \param images the images to insert as references into the respective correspondences
     * \param objectModels the object models to insert as reference into the respective correspondence
     * \return the list of all stored correspondences
     * \throws an exception if the path to the folder that should hold the correspondences has not been set previously
     */
    vector<ObjectImageCorrespondence> loadCorrespondences(vector<Image>* images, vector<ObjectModel>* objectModels);

    /*!
     * \brief setImagesPath Sets the path to the folder where the images that are to be annotated are located. After setting the
     * path the manager will automatically try to load the images and also their respective segmentation using the pattern for
     * segmentation image names.
     * \param path the path to the folder where the images are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setImagesPath(path path);

    /*!
     * \brief getImagesPath Returns the path that this manager uses to load images.
     * \return the path that this manager uses to load images
     */
    path getImagesPath() const;

    /*!
     * \brief setObjectModelsPath Sets the path to the folder where the object models are located. After setting the path the
     * manager will automatically try to load the objects.
     * \param path the path to the folder where the objects are located
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setObjectModelsPath(path path);

    /*!
     * \brief getObjectModelsPath Returns the path that this manager uses to load object models.
     * \return the path that this manager uses to load object models
     */
    path getObjectModelsPath() const;

    /*!
     * \brief setCorrespondencesPath Sets the path to the folder where object image correspondences are stored at. After setting
     * the path the manager will automatically try to load the already existing correspondences and will store all future
     * correspondences at this location.
     * \param path the path where the manager should store correspondences at or where some correspondences already exist that
     * are meant to be loaded
     * \return true if the path is a valid path on the filesystem, false otherwise
     */
    bool setCorrespondencesPath(path path);

    /*!
     * \brief getCorrespondencesPath Returns the path that this manager uses to store and load correspondences.
     * \return the path that this manager uses to store and load correspondences
     */
    path getCorrespondencesPath() const;

    /*!
     * \brief setSegmentationImageFilesSuffix sets the given suffix as the suffix to be used
     * when loading segmentation images. Segmentation images have to be in the same folder as
     * the actual images.
     * \param suffix the suffix to be used for segmentation images
     */
    void setSegmentationImageFilesSuffix(string suffix);

    /*!
     * \brief getSegmentationImageFilesSuffix returns the suffix that is used to load
     * segementation images from the folder of images.
     * \return the suffix that is used to load segmentation images
     */
    string getSegmentationImageFilesSuffix();

    /*!
     * \brief setImageFilesExtension sets the extension to be used to load images.
     * \param extension the extension to be used to load images.
     */
    void setImageFilesExtension(string extension);

    /*!
     * \brief getImageFilesExtension returns the currently set extension of the image files
     * that are to be loaded.
     * \return the extension of the image files
     */
    string getImageFilesExtension();
};

#endif // TEXTFILELOADANDSTORESTRATEGY_H
