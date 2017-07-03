#include "textfileloadandstorestrategy.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

const char correspondenceFormatDelimiter = ' ';
const string correspondenceFileNameSuffix = "_correspondence";

TextFileLoadAndStoreStrategy::TextFileLoadAndStoreStrategy(const string _imagesPath, const string _objectModelsPath,
                                                           const string _correspondencesPath)
    : imagesPath(_imagesPath), objectModelsPath(_objectModelsPath), correspondencesPath(_correspondencesPath) {
    if (!pathExists(_imagesPath))
        throw "Image path does not exist";
    if (!pathExists(_objectModelsPath))
        throw "ObjectModels path does not exist";
    if (!pathExists(_correspondencesPath))
        throw "Correspondences path does not exist";
}

TextFileLoadAndStoreStrategy::~TextFileLoadAndStoreStrategy() {
}

list<string> getFilesAtPath(const string path) {
    list<string> files;

    boost::filesystem::path _path(path);
    boost::filesystem::directory_iterator end_itr;

    for (boost::filesystem::directory_iterator itr(_path); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path())) {
            string current_file = itr->path().string();
            files.push_back(current_file);
        }
    }

    return files;
}

bool TextFileLoadAndStoreStrategy::persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) {
    const string imagePath = objectImageCorrespondence.getImage()->getPath();
    boost::filesystem::path image(imagePath);

    string correspondenceFileName = correspondencesPath + image.stem().string() + correspondenceFileNameSuffix + ".txt";
    boost::filesystem::path _path(correspondenceFileName);
    if (deleteCorrespondence) {
        std::remove(correspondenceFileName.c_str());
    } else if (boost::filesystem::exists(_path) && boost::filesystem::is_regular_file(_path)) {
        //! file exists already we need to update it
        ifstream inFile(correspondenceFileName);
        //! we need to create a temproary file and replace the old version with the new one at the end
        ofstream outFile("./tmp.txt");
        string line;
        bool correspondenceFound = false;
        while (inFile >> line) {
            std::vector<std::string> results;
            //! split the read line at the delimiter to obtain the ID of the correspondence and see if this is the line we need to update
            boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
            if (results[0].compare(objectImageCorrespondence.getID()) == 0) {
                //! we found the correct line
                correspondenceFound = true;
                outFile << objectImageCorrespondence.toString() << endl;
            } else {
                outFile << line << endl;
            }
        }
        if (!correspondenceFound)
            //! since we didn't find the correspondence it must be a new one and we add it at the end of the file
            outFile << objectImageCorrespondence.toString();

        remove(correspondenceFileName.c_str());
        boost::filesystem::copy_file(boost::filesystem::path("./tmp.txt"), boost::filesystem::path(correspondenceFileName));
        inFile.close();
        outFile.close();
    } else {
        //! no file there yet so we have to create it
        ofstream outFile(correspondenceFileName);
        outFile << objectImageCorrespondence.toString();
        outFile.close();
    }

    return true;
}

list<Image> TextFileLoadAndStoreStrategy::loadImages() {
    if (!pathExists(imagesPath)) {
        throw "Path does not exist";
    }

    list<Image> images;
    list<string> filesAtPath = getFilesAtPath(imagesPath);
    for (list<string>::iterator itr = filesAtPath.begin(); itr != filesAtPath.end(); ++itr) {
        //! read in every second image as segmentation image
        // TODO: implement algorithm to accept pattern for segmentation images
        string imagePath = (*itr);
        ++itr;
        string segmentationImagePath = (*itr);
        images.push_back(Image(imagePath, segmentationImagePath));
    }

    return images;
}

list<ObjectModel> TextFileLoadAndStoreStrategy::loadObjectModels() {
    if (!pathExists(objectModelsPath)) {
        throw "Path does not exist";
    }

    list<ObjectModel> objectModels;
    for (auto file : getFilesAtPath(objectModelsPath)) {
        objectModels.push_back(ObjectModel(file));
    }
    return objectModels;
}

map<string, Image*> createImageMap(list<Image>* images) {
    map<string, Image*> imageMap;

    for (auto image : *images) {
        Image _image = image;
        imageMap[_image.getPath()] = &_image;
    }

    return imageMap;
}

map<string, ObjectModel*> createObjectModelMap(list<ObjectModel>* objectModels) {
    map<string, ObjectModel*> objectModelMap;

    for (auto objectModel : *objectModels) {
        ObjectModel _objectModel = objectModel;
        objectModelMap[_objectModel.getPath()] = &_objectModel;
    }

    return objectModelMap;
}

list<ObjectImageCorrespondence> TextFileLoadAndStoreStrategy::loadCorrespondences(list<Image>* images, list<ObjectModel>* objectModels) {
    if (!pathExists(correspondencesPath)) {
        throw "Path does not exist";
    }

    map<string, Image*> imageMap = createImageMap(images);
    map<string, ObjectModel*> objectModelMap = createObjectModelMap(objectModels);


    list<ObjectImageCorrespondence> correspondences;
    ifstream inFile;
    for (auto file : getFilesAtPath(correspondencesPath)) {
        string _file = file;
        //! only read txt files
        //!
        if (strcmp(_file.substr(_file.size() - 4, 4).c_str(), ".txt") == 0) {
            inFile.open(_file);
            string line;
            while (inFile >> line) {
                std::vector<std::string> results;
                //! split the read line at the delimiter and parse the results
                boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
                correspondences.push_back(ObjectImageCorrespondence(results[0], stoi(results[3]), stoi(results[4]),
                        stoi(results[5]), stoi(results[6]), stoi(results[7]), stoi(results[8]), imageMap[results[1]],
                        objectModelMap[results[2]]));
            }
            inFile.close();
        }
    }

    return correspondences;
}



bool TextFileLoadAndStoreStrategy::setImagesPath(string path) {
    if (!this->pathExists(path))
        return false;
    if (imagesPath == path)
        return true;

    imagesPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        listener->imagesChanged();
    }
    return true;
}

string TextFileLoadAndStoreStrategy::getImagesPath() const {
    return  imagesPath;
}

bool TextFileLoadAndStoreStrategy::setObjectModelsPath(string path) {
    if (!this->pathExists(path))
        return false;
    if (objectModelsPath == path)
        return true;

    objectModelsPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        listener->objectModelsChanged();
    }
    return true;
}

string TextFileLoadAndStoreStrategy::getObjectModelsPath() const {
    return objectModelsPath;
}

bool TextFileLoadAndStoreStrategy::setCorrespondencesPath(string path) {
    if (!this->pathExists(path))
        return false;
    if (correspondencesPath == path)
        return true;

    correspondencesPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        listener->corresopndencesChanged();
    }
    return false;
}

string TextFileLoadAndStoreStrategy::getCorrespondencesPath() const {
    return correspondencesPath;
}
