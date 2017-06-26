#ifndef LOADANDSTORESTRATEGY_H
#define LOADANDSTORESTRATEGY_H

#endif // LOADANDSTORESTRATEGY_H

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
    /*!
     * \brief persistObjectImageCorrespondence Persists the given ObjectImageCorrespondence at the given path. The details of
     * how the data is persisted depends on the LoadAndStoreStrategy implementation.
     * \param objectImageCorrespondence the object image correspondence to persist
     * \param path the path at which the object image correspondence is to be persisted
     * \param deleteCorrespondence indicates whether the correspondence should be persistently deleted
     * \return true if persisting the object image correspondence was successful, false if not
     */
    bool persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, string path, bool deleteCorrespondence);

    bool loadEntities(const string imagesPath, list<Image>* images, const string objectModelsPath, list<ObjectModel>* objectModels,
                      const string correspondencesPath, list<ObjectImageCorrespondence>* correspondences);

    /*!
     * \brief pathExists Checks whether the given path exists on the file system and is accessible.
     * \param path the path that is to be checked
     * \return true if the path exists and is accessible, false if not
     */
    bool pathExists(const string path);

};
