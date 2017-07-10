#include "textfileloadandstorestrategy.hpp"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

const char correspondenceFormatDelimiter = ' ';
const string correspondenceFileNameSuffix = "_correspondence";
const string correspondenceFileExtension = ".txt";

TextFileLoadAndStoreStrategy::TextFileLoadAndStoreStrategy(const path _imagesPath, const path _objectModelsPath,
                                                           const path _correspondencesPath)
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

list<string> getFilesAtPath(const path _path) {
    list<string> files;

    boost::filesystem::directory_iterator end_itr;

    for (boost::filesystem::directory_iterator itr(_path); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path())) {
            string current_file = itr->path().string();
            files.push_back(current_file);
        }
    }

    return files;
}

void persistCorrespondenceToFile(ifstream *inFile, ofstream *outFile, const ObjectImageCorrespondence* correspondence, bool deleteCorrespondence) {
    bool correspondenceFound = false;
    for (string line; getline(*inFile, line);) {
        std::vector<std::string> results;
        //! split the read line at the delimiter to obtain the ID of the correspondence and see if this is the line we need to update
        boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
        if (results[0].compare(correspondence->getID()) == 0) {
            //! we found the correct line
            correspondenceFound = true;
            if (!deleteCorrespondence)
                //! if we are to delete the correspondence, simply don't write back the line it concerns
                *outFile << correspondence->toString() << endl;
        } else {
            *outFile << line << endl;
        }
    }
    if (!correspondenceFound && !deleteCorrespondence)
        //! since we didn't find the correspondence it must be a new one and we add it at the end of the file
        //! but only if the delete flag isn't set to true
        *outFile << correspondence->toString();
}

bool TextFileLoadAndStoreStrategy::persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) {
    boost::filesystem::path imagePath(objectImageCorrespondence.getImage()->getImagePath());
    boost::filesystem::path imageFileName(imagePath.stem().string() + correspondenceFileNameSuffix + correspondenceFileExtension);
    boost::filesystem::path corresopndenceFileName = absolute(imageFileName, correspondencesPath);
    if (boost::filesystem::exists(corresopndenceFileName) && boost::filesystem::is_regular_file(corresopndenceFileName)) {
        //! file exists already we need to update it
        ifstream inFile(corresopndenceFileName.string());
        //! we need to create a temproary file and replace the old version with the new one at the end
        ofstream outFile("./tmp.txt");
        persistCorrespondenceToFile(&inFile, &outFile, &objectImageCorrespondence, deleteCorrespondence);
        boost::filesystem::copy_file(boost::filesystem::path("./tmp.txt"), corresopndenceFileName);
        inFile.close();
        outFile.close();
        remove("./tmp.txt");
    } else {
        //! no file there yet so we have to create it
        ofstream outFile(corresopndenceFileName.string());
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

    //for (int i = 0; i < images.size(); i++) {
        //imageMap["((Image) images[i]).getPath"] = &(images[i]);
    //}

    return imageMap;
}

map<string, ObjectModel*> createObjectModelMap(list<ObjectModel>* objectModels) {
    map<string, ObjectModel*> objectModelMap;

   // for (int i = 0; i < objectModels.size(); i++) {
        //objectModelMap[objectModel.getPath()] = &objectModel;
    //}

    return objectModelMap;
}

void loadCorrespondencesFromFile(ifstream* inFile, list<ObjectImageCorrespondence> *correspondences,
                       map<string, Image*>* imageMap, map<string, ObjectModel*> *objectModelMap) {
    if (inFile->is_open()) {
        //! read every line
        for (string line; getline(*inFile, line);) {
            std::vector<std::string> results;
            //! split the read line at the delimiter and parse the results
            boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
            //! add the correspondence parsed from the line of the file to the list of correspondences
            correspondences->push_back(ObjectImageCorrespondence(results[0], stoi(results[3]), stoi(results[4]),
                    stoi(results[5]), stoi(results[6]), stoi(results[7]), stoi(results[8]), (*imageMap)[results[1]],
                    (*objectModelMap)[results[2]]));
        }
    }
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
            loadCorrespondencesFromFile(&inFile, &correspondences, &imageMap, &objectModelMap);
            inFile.close();
        }
    }

    return correspondences;
}



bool TextFileLoadAndStoreStrategy::setImagesPath(path path) {
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

path TextFileLoadAndStoreStrategy::getImagesPath() const {
    return  imagesPath;
}

bool TextFileLoadAndStoreStrategy::setObjectModelsPath(path path) {
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

path TextFileLoadAndStoreStrategy::getObjectModelsPath() const {
    return objectModelsPath;
}

bool TextFileLoadAndStoreStrategy::setCorrespondencesPath(path path) {
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

path TextFileLoadAndStoreStrategy::getCorrespondencesPath() const {
    return correspondencesPath;
}
