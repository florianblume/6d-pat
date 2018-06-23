#include "image.hpp"
#include <QDir>

Image::Image()
    : imagePath("invalid"),
      segmentationImagePath("invalid"),
      basePath("invalid"),
      cameraMatrix() {

}

Image::Image(const QString& imagePath, const QString& basePath, QMatrix3x3 cameraMatrix)
    : imagePath(imagePath),
      segmentationImagePath(""),
      basePath(basePath),
      cameraMatrix(cameraMatrix) {
}

Image::Image(const QString& imagePath, const QString& basePath,
             const QString& segmentationImagePath, QMatrix3x3 cameraMatrix)
    : imagePath(imagePath),
      segmentationImagePath(segmentationImagePath),
      basePath(basePath),
      cameraMatrix(cameraMatrix) {
}

Image::Image(const Image &other) {
    imagePath = other.imagePath;
    segmentationImagePath = other.segmentationImagePath;
    basePath = other.basePath;
    cameraMatrix = other.cameraMatrix;
}

QString Image::getImagePath() const {
    return imagePath;
}

QString Image::getAbsoluteImagePath() const {
    return QDir(basePath).filePath(imagePath);
}

QString Image::getSegmentationImagePath() const {
    return segmentationImagePath;
}

QString Image::getAbsoluteSegmentationImagePath() const {
    return QDir(basePath).filePath(segmentationImagePath);
}

QString Image::getBasePath() const {
    return basePath;
}

QMatrix3x3 Image::getCameraMatrix() const {
    return cameraMatrix;
}

bool Image::operator==(const Image &other) {
    // QString supports standard string comparison ==
    return basePath == other.basePath &&
            imagePath == other.imagePath &&
            segmentationImagePath == other.segmentationImagePath &&
            cameraMatrix == other.cameraMatrix;
}

Image& Image::operator=(const Image &other) {
    basePath = other.basePath;
    imagePath = other.imagePath;
    segmentationImagePath = other.segmentationImagePath;
    cameraMatrix = other.cameraMatrix;
    return *this;
}
