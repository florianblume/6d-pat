#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#include "objectimagecorrespondence.hpp"
#include "loadandstorestrategylistener.hpp"
#include "image.hpp"
#include "objectmodel.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class LoadAndStoreStrategy {

protected:
    vector<LoadAndStoreStrategyListener*> listeners;

public:

    virtual ~LoadAndStoreStrategy();

    /*!
     * \brief persistObjectImageCorrespondence Persists the given ObjectImageCorrespondence. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImageCorrespondence the object image correspondence to persist
     * \param deleteCorrespondence indicates whether the correspondence should be persistently deleted
     * \return true if persisting the object image correspondence was successful, false if not
     */
    virtual bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) = 0;

    /*!
     * \brief loadImages Loads the images.
     * \return the list of images
     */
    virtual vector<Image> loadImages() = 0;

    /*!
     * \brief loadObjectModels Loads the object models.
     * \return the list of object models
     */
    virtual vector<ObjectModel> loadObjectModels() = 0;

    /*!
     * \brief loadCorrespondences Loads the correspondences at the given path. How the correspondences are stored depends on the
     * strategy.
     * \param images the images to insert as references into the respective correspondences
     * \param objectModels the object models to insert as reference into the respective correspondence
     * \return the list of all stored correspondences
     */
    virtual vector<ObjectImageCorrespondence> loadCorrespondences(vector<Image>* images, vector<ObjectModel>* objectModels) = 0;

    /*!
     * \brief pathExists Checks whether the given path exists on the file system and is accessible.
     * \param path the path that is to be checked
     * \return true if the path exists and is accessible, false if not
     */
    bool pathExists(const path path);

    /*!
     * \brief addListener Adds a listener to this load and store strategy that will be notified if the underlying data changes somehow.
     * \param listener the listener to be added
     */
    void addListener(LoadAndStoreStrategyListener* listener);

    /*!
     * \brief removeListener Removes the listener from the listeners of this strategy.
     * \param listener the listener to be removed
     */
    void removeListener(LoadAndStoreStrategyListener* listener);

};

#endif // LOADANDSTORESTRATEGY_H
