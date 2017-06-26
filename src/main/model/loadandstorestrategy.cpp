#include "loadandstorestrategy.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

const char correspondenceFormatDelimiter = ' ';
const string correspondenceFileNameSuffix = "_correspondence";

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

bool LoadAndStoreStrategy::persistObjectImageCorrespondence(const ObjectImageCorrespondence& objectImageCorrespondence, string path, bool deleteCorrespondence) {
    if (!pathExists(path))
        return false;

    const string imagePath = objectImageCorrespondence.getImage()->getPath();
    boost::filesystem::path image(imagePath);

    string correspondenceFileName = path + image.stem().string() + correspondenceFileNameSuffix + ".txt";
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

bool LoadAndStoreStrategy::loadEntities(const string imagesPath, list<Image>* images, const string objectModelsPath, list<ObjectModel>* objectModels,
                  const string correspondencesPath, list<ObjectImageCorrespondence>* correspondences) {
    if (!pathExists(imagesPath) || !pathExists(objectModelsPath) || !pathExists(correspondencesPath))
        return false;

    map<string, Image*> imageMap;
    map<string, ObjectModel*> objectModelMap;

    //! load images
    list<string> filesAtPath = getFilesAtPath(imagesPath);
    for (list<string>::iterator itr = filesAtPath.begin(); itr != filesAtPath.end(); ++itr) {
        //! read in every second image as segmentation image
        // TODO: implement algorithm to accept pattern for segmentation images
        string imagePath = (*itr);
        ++itr;
        string segmentationImagePath = (*itr);
        images->push_back(Image(imagePath, segmentationImagePath));
        imageMap[imagePath] = &(*images->end());
    }

    //! load object models
    for (auto file : getFilesAtPath(objectModelsPath)) {
        objectModels->push_back(ObjectModel(file));
        objectModelMap[file] = &(*objectModels->end());
    }

    //! load correspondences
    ifstream inFile;
    for (auto file : getFilesAtPath(correspondencesPath)) {
        string _file = file;
        //! only read txt files
        //!
        if (strcmp(_file.substr(_file.size() - 5, 4).c_str(), ".txt") == 0) {
            inFile.open(_file);
            string line;
            while (inFile >> line) {
                std::vector<std::string> results;
                //! split the read line at the delimiter and parse the results
                boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
                correspondences->push_back(ObjectImageCorrespondence(results[0], stoi(results[3]), stoi(results[4]),
                        stoi(results[5]), stoi(results[6]), stoi(results[7]), stoi(results[8]), imageMap[results[1]],
                        objectModelMap[results[2]]));
            }
            inFile.close();
        }
    }

    return true;
}

bool LoadAndStoreStrategy::pathExists(const string path) {
    boost::filesystem::path _path(path);
    return boost::filesystem::is_directory(_path);
}
