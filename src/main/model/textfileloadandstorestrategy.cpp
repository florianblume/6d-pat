#include "textfileloadandstorestrategy.hpp"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

const char correspondenceFormatDelimiter = ' ';
const string correspondenceFilesNameSuffix = "_correspondence";
const string correspondenceFilesExtension = ".txt";
const string imageFilesExtension = ".png";
const string objectModelFilesExtension = "";
const string segmentationImageFilesSuffix = "_GT";

TextFileLoadAndStoreStrategy::TextFileLoadAndStoreStrategy() {
    // nothing to do here
}

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

static boost::filesystem::path convertPathToSuffxFileName(const path pathToConvert, const string suffix, const string extension) {
    return boost::filesystem::path(pathToConvert.stem().string() + suffix + extension);
}

static vector<string> getFilesAtPath(const path _path, const string extension) {
    vector<string> files;

    boost::filesystem::directory_iterator end_itr;

    for (boost::filesystem::directory_iterator itr(_path); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path()) && itr->path().extension() == extension) {
            string current_file = itr->path().string();
            files.push_back(current_file);
        }
    }

    sort(files.begin(), files.end());

    return files;
}

static void persistCorrespondenceToFile(ifstream *inFile, ofstream *outFile,
                                        const ObjectImageCorrespondence* correspondence, bool deleteCorrespondence) {
    bool correspondenceFound = false;
    //! count how many lines are not this correspondence, if there aren't any other and this correspondence is to be deleted, remove the file
    uint foreignLinesCount = 0;
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
            foreignLinesCount++;
        }
    }
    if (!correspondenceFound && !deleteCorrespondence) {
        //! since we didn't find the correspondence it must be a new one and we add it at the end of the file
        //! but only if the delete flag isn't set to true
        *outFile << correspondence->toString();
    } else if (foreignLinesCount == 0 && deleteCorrespondence) {
        //TODO: remove file
    }
}

bool TextFileLoadAndStoreStrategy::persistObjectImageCorrespondence(
        const ObjectImageCorrespondence& objectImageCorrespondence, bool deleteCorrespondence) {
    // no need to check whether paths exist because setters do so already

    boost::filesystem::path imagePath(objectImageCorrespondence.getImage()->getImagePath());
    boost::filesystem::path correspondenceFileName = convertPathToSuffxFileName(imagePath,
                                                                              correspondenceFilesNameSuffix,
                                                                              correspondenceFilesExtension);
    boost::filesystem::path correspondenceFilePath = absolute(correspondenceFileName, correspondencesPath);

    if (boost::filesystem::exists(correspondenceFilePath) && boost::filesystem::is_regular_file(correspondenceFilePath)) {
        //! remove an already existing temp files
        boost::filesystem::path tempPath("./tmp");
        if (boost::filesystem::exists(tempPath))
            boost::filesystem::remove(tempPath);

        //! file exists already we need to update it
        ifstream inFile(correspondenceFilePath.string());
        //! we need to create a temproary file and replace the old version with the new one at the end
        ofstream outFile("./tmp");
        persistCorrespondenceToFile(&inFile, &outFile, &objectImageCorrespondence, deleteCorrespondence);
        //! remove the old file and copy the content of the temporary file at the same location
        boost::filesystem::remove(correspondenceFilePath);
        boost::filesystem::copy_file(tempPath, correspondenceFilePath);
        inFile.close();
        outFile.close();
    } else {
        //! no file there yet so we have to create it
        ofstream outFile(correspondenceFilePath.string());
        outFile << objectImageCorrespondence.toString();
        outFile.close();
    }

    return true;
}

vector<Image> TextFileLoadAndStoreStrategy::loadImages() {
    // no need to check whether paths exist because setters do so already

    vector<Image> images;
    vector<string> filesAtPath = getFilesAtPath(imagesPath, imageFilesExtension);
    for (vector<string>::iterator itr = filesAtPath.begin(); itr != filesAtPath.end(); ++itr) {
        //! read in every second image as segmentation image
        string imagePathString = (*itr);
        boost::filesystem::path imagePath(imagePathString);
        vector<string>::iterator _itr = itr;
        //! check if next image complies to pattern of segmentation image
        ++_itr;
        string segmentationImagePathString = (*_itr);
        boost::filesystem::path segmentationImagePath(segmentationImagePathString);
        //! we only need the filename of the segmentation image not the full path
        segmentationImagePath = segmentationImagePath.filename();
        boost::filesystem::path expectedSegmentationImagePath = convertPathToSuffxFileName(
                    imagePath, segmentationImageFilesSuffix, imageFilesExtension);
        if (segmentationImagePath.string().compare(expectedSegmentationImagePath.string()) == 0) {
            //! we found a segmentation image, i.e. the next image complied to the expected pattern
            //! we need to set forward the iterator to skip the segmentation image that we're going to add here
            ++itr;
            images.push_back(Image(imagePath.filename(), imagesPath, segmentationImagePath));
        } else {
            //!
            //! create new image with the filename of the image and the path to it's containing folder
            images.push_back(Image(imagePath.filename(), imagesPath));
        }
    }

    return images;
}

vector<ObjectModel> TextFileLoadAndStoreStrategy::loadObjectModels() {
    if (!pathExists(objectModelsPath)) {
        throw "Object models path does not exist";
    }

    vector<ObjectModel> objectModels;
    for (auto file : getFilesAtPath(objectModelsPath, objectModelFilesExtension)) {
        boost::filesystem::path _file(file);
        objectModels.push_back(ObjectModel(_file.filename(), objectModelsPath));
    }
    return objectModels;
}

map<string, Image*> createImageMap(vector<Image>* images) {
    map<string, Image*> imageMap;

    for (uint i = 0; i < images->size(); i++) {
        imageMap[images->at(i).getImagePath().string()] = &(images->at(i));
    }

    return imageMap;
}

map<string, ObjectModel*> createObjectModelMap(vector<ObjectModel>* objectModels) {
    map<string, ObjectModel*> objectModelMap;

    for (uint i = 0; i < objectModels->size(); i++) {
        objectModelMap[objectModels->at(i).getPath().string()] = &(objectModels->at(i));
    }

    return objectModelMap;
}

void loadCorrespondencesFromFile(ifstream* inFile, vector<ObjectImageCorrespondence> *correspondences,
                       map<string, Image*>* imageMap, map<string, ObjectModel*> *objectModelMap) {
    if (inFile->is_open()) {
        //! read every line
        for (string line; getline(*inFile, line);) {
            std::vector<std::string> results;
            //! split the read line at the delimiter and parse the results
            boost::split(results, line, [](char c){return c == correspondenceFormatDelimiter;});
            //! add the correspondence parsed from the line of the file to the list of correspondences
            Image* image = (*imageMap)[results[1]];
            ObjectModel* objectModel = (*objectModelMap)[results[2]];
            if (image && objectModel && results.size() == 10) {
                correspondences->push_back(ObjectImageCorrespondence(results[0], stoi(results[3]), stoi(results[4]),
                    stoi(results[5]), stoi(results[6]), stoi(results[7]), stoi(results[8]), stof(results[9]), image, objectModel));
            } else {
                throw "Could not find corresponding image or object model within the given entity-lists.";
            }
        }
    }
}

vector<ObjectImageCorrespondence> TextFileLoadAndStoreStrategy::loadCorrespondences(
        vector<Image>* images, vector<ObjectModel>* objectModels) {
    // no need to check whether paths exist because setters do so already
    map<string, Image*> imageMap = createImageMap(images);
    map<string, ObjectModel*> objectModelMap = createObjectModelMap(objectModels);


    vector<ObjectImageCorrespondence> correspondences;
    ifstream inFile;
    for (auto file : getFilesAtPath(correspondencesPath, correspondenceFilesExtension)) {
        string _file = file;
        inFile.open(_file);
        loadCorrespondencesFromFile(&inFile, &correspondences, &imageMap, &objectModelMap);
        inFile.close();
    }

    return correspondences;
}



bool TextFileLoadAndStoreStrategy::setImagesPath(path path) {
    if (!this->pathExists(path))
        throw "Images path does not exist";
    if (imagesPath == path)
        return true;

    imagesPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        if (listener)
            listener->imagesChanged();
    }
    return true;
}

path TextFileLoadAndStoreStrategy::getImagesPath() const {
    return  imagesPath;
}

bool TextFileLoadAndStoreStrategy::setObjectModelsPath(path path) {
    if (!this->pathExists(path))
        throw "Object models path does not exist";
    if (objectModelsPath == path)
        return true;

    objectModelsPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        if (listener)
            listener->objectModelsChanged();
    }
    return true;
}

path TextFileLoadAndStoreStrategy::getObjectModelsPath() const {
    return objectModelsPath;
}

bool TextFileLoadAndStoreStrategy::setCorrespondencesPath(path path) {
    if (!this->pathExists(path))
        throw "Correspondences path does not exist";
    if (correspondencesPath == path)
        return true;

    correspondencesPath = path;
    for (LoadAndStoreStrategyListener *listener : listeners) {
        if (listener)
            listener->corresopndencesChanged();
    }
    return true;
}

path TextFileLoadAndStoreStrategy::getCorrespondencesPath() const {
    return correspondencesPath;
}
