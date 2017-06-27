#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "objectimagecorrespondence.h"
#include "image.h"
#include "objectmodel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

class LoadAndStoreStrategy {

public:

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief persistObjectImageCorrespondence Persists the given ObjectImageCorrespondence at the given path. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImageCorrespondence the object image correspondence to persist
     * \param path the path at which the object image correspondence is to be persisted
     * \param deleteCorrespondence indicates whether the correspondence should be persistently deleted
     * \return true if persisting the object image correspondence was successful, false if not
     */
    virtual bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, string path, bool deleteCorrespondence) = 0;

    /*!
     * \brief loadImages Loads the images at the given path.
     * \param imagesPath the path to load the images from
     * \return the list of images at the given path
     * \throws an exception if the path doesn't exist
     */
    virtual list<Image> loadImages(const string imagesPath) = 0;

    /*!
     * \brief loadObjectModels Loads the object models at the given path.
     * \param objectModelsPath the path to load the object models from
     * \return the list of object models at the given path
     * \throws an exception if the path doesn't exist
     */
    virtual list<ObjectModel> loadObjectModels(const string objectModelsPath) = 0;

    /*!
     * \brief loadCorrespondences Loads the correspondences at the given path. How the correspondences are stored depends on the
     * strategy.
     * \param images the images to insert as references into the respective correspondences
     * \param objectModels the object models to insert as reference into the respective correspondence
     * \return the list of all stored correspondences
     */
    virtual list<ObjectImageCorrespondence> loadCorrespondences(list<Image>* images, list<ObjectModel>* objectModels) = 0;

    /*!
     * \brief pathExists Checks whether the given path exists on the file system and is accessible.
     * \param path the path that is to be checked
     * \return true if the path exists and is accessible, false if not
     */
    bool pathExists(const string path);

};

#endif // LOADANDSTORESTRATEGY_H
