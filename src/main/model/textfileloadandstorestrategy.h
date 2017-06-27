#ifndef TEXTFILELOADANDSTORESTRATEGY_H
#define TEXTFILELOADANDSTORESTRATEGY_H

#include "loadandstorestrategy.h"

/*!
 * \brief The TextFileLoadAndStoreStrategy class This class is a simple implementation of a LoadAndStoreStrategy that makes no use of
 * caching already loaded images or object models and writes correspondences to text files. That is why it is important to set the
 * proper path to the folder of correspondences before using this strategy.
 */
class TextFileLoadAndStoreStrategy : public LoadAndStoreStrategy
{
private:
    //! The path to the folder that stores the correspondences
    string correspondencesPath;

public:
    TextFileLoadAndStoreStrategy(const string _correspondencesPath);
    ~TextFileLoadAndStoreStrategy();
    bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, string path, bool deleteCorrespondence);
    list<Image> loadImages(const string imagesPath);
    list<ObjectModel> loadObjectModels(const string objectModelsPath);

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
    list<ObjectImageCorrespondence> loadCorrespondences(list<Image>* images, list<ObjectModel>* objectModels);

    /*!
     * \brief setCorrespondencesPath Sets the path to read from and store to all correspondences. This is necessary because this strategy
     * uses text files to store correspondences.
     * \param correspondencesPath the path to the folder that should hold the correspondences
     * \return true if the path exists and is accessible
     */
    bool setCorrespondencesPath(const string correspondencesPath);

    /*!
     * \brief getCorrespondencesPath Returns the path to the folder that holds the correspondences and is used to store them.
     * \return the path to the folder of correspondences
     */
    string getCorrespondencesPath() const;
};

#endif // TEXTFILELOADANDSTORESTRATEGY_H
