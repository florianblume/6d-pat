#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "objectimagecorrespondence.h"
#include "image.h"
#include <string>
#include <list>

using namespace std;

//! Class ModelManager manages data throughout the program and takes care of persistency.
/*!
 * A ModelManager is there to read in images and 3D models as well as correspondences already created by the user. It does so automatically on
 * startup and it also takes care of persisting changes constantly.
 * A ModelManager can also provide the read images, object models and correspondences.
*/
class ModelManager
{
private:
    string imagesPath;
    string objectModelsPath;
    string annotationsPath;
    list<Image> images;
    list<ObjectModel> objectModels;
    list<ObjectImageCorrespondence> correspondences;

    void storeData();
    void readData();

public:
    ModelManager();
    void init();
    bool setImagesPath(string path);
    string getImagesPath();
    bool setObjectModelsPath(string path);
    string getObjectModelsPath();
    bool setCorrespondencesPath(string path);
    string getCorrespondencesPath();

    const list<Image>* getImages();
    const list<ObjectModel>* getObjectModels();
    const list<ObjectImageCorrespondence>* getCorrespondences();

    bool addObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence);
    bool updateObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence);
    bool removeObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence);

};

#endif // MODELMANAGER_H
