#include "image.hpp"
#include <QDir>

Image::Image(const QString& imagePath, const QString& basePath)
    : imagePath(imagePath),
      segmentationImagePath(""),
      basePath(basePath) {
}

Image::Image(const QString& imagePath, const QString& basePath, const QString& segmentationImagePath)
    : imagePath(imagePath),
      segmentationImagePath(segmentationImagePath),
      basePath(basePath) {
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

Image& Image::operator=(Image other) {
    std::swap(*this, other);
    return *this;
}
