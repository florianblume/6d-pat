#include "image.hpp"
#include <QDir>

Image::Image(const QString& imagePath, const QString& basePath,
             float focalLenghtX, float focalLenghtY, float focalPointX, float focalPointY)
    : imagePath(imagePath),
      segmentationImagePath(""),
      basePath(basePath),
      focalLenghtX(focalLenghtX),
      focalLenghtY(focalLenghtY),
      focalPointX(focalPointX),
      focalPointY(focalPointY) {
}

Image::Image(const QString& imagePath, const QString& basePath, const QString& segmentationImagePath,
             float focalLenghtX, float focalLenghtY, float focalPointX, float focalPointY)
    : imagePath(imagePath),
      segmentationImagePath(segmentationImagePath),
      basePath(basePath),
      focalLenghtX(focalLenghtX),
      focalLenghtY(focalLenghtY),
      focalPointX(focalPointX),
      focalPointY(focalPointY) {
}

Image::Image(const Image &other) {
    imagePath = other.imagePath;
    segmentationImagePath = other.segmentationImagePath;
    basePath = other.basePath;
    focalLenghtX = other.focalLenghtX;
    focalLenghtY = other.focalLenghtY;
    focalPointX = other.focalPointX;
    focalPointY = other.focalPointY;
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

float Image::getFocalLengthX() {
    return focalLenghtX;
}

float Image::getFocalLengthY() {
    return focalLenghtY;
}

float Image::getFocalPointX() {
    return focalPointX;
}

float Image::getFocalPointY() {
    return focalPointY;
}

bool Image::operator==(const Image &other) {
    // QString supports standard string comparison ==
    return basePath == other.basePath &&
            imagePath == other.imagePath &&
            segmentationImagePath == other.segmentationImagePath &&
            focalLenghtX == other.focalLenghtX &&
            focalLenghtY == other.focalLenghtY &&
            focalPointX == other.focalPointX &&
            focalPointY == other.focalPointY;
}

Image& Image::operator=(const Image &other) {
    basePath = other.basePath;
    imagePath = other.imagePath;
    segmentationImagePath = other.segmentationImagePath;
    return *this;
}
