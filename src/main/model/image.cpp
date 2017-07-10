#include "image.hpp"

Image::Image(const path _path, const path _basePath) : imagePath(_path), segmentationImagePath(""), basePath(_basePath) {
}

Image::Image(const path _path, const path _basePath, const path _segmentationPath)
    : imagePath(_path), segmentationImagePath(_segmentationPath), basePath(_basePath) {
}

const path Image::getImagePath() const {
    return imagePath;
}

const path Image::getAbsoluteImagePath() const {
    return absolute(imagePath, basePath);
}

const path Image::getSegmentationImagePath() const {
    return segmentationImagePath;
}

const path Image::getAbsoluteSegmentationImagePath() const {
    return absolute(segmentationImagePath, basePath);
}

const path Image::getBasePath() const {
    return basePath;
}
