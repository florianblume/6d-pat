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

Image& Image::operator=(Image other) {
    std::swap(*this, other);
    return *this;
}
