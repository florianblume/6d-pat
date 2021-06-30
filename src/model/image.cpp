#include "image.hpp"
#include <QDir>

Image::Image()
    : m_imagePath("invalid"),
      m_segmentationImagePath("invalid"),
      m_basePath("invalid"),
      m_cameraMatrix() {

}

Image::Image(const QString &id, const QString& imagePath,
             const QString& basePath, QMatrix3x3 cameraMatrix,
             float nearPlane, float farPlane)
    : m_id(id),
      m_imagePath(imagePath),
      m_segmentationImagePath(""),
      m_basePath(basePath),
      m_cameraMatrix(cameraMatrix),
      m_nearPlane(nearPlane),
      m_farPlane(farPlane) {
}

Image::Image(const QString &id, const QString& imagePath, const QString& segmentationImagePath,
             const QString& basePath, QMatrix3x3 cameraMatrix,
             float nearPlane, float farPlane)
    : m_id(id),
      m_imagePath(imagePath),
      m_segmentationImagePath(segmentationImagePath),
      m_basePath(basePath),
      m_cameraMatrix(cameraMatrix),
      m_nearPlane(nearPlane),
      m_farPlane(farPlane) {
}

Image::Image(const Image &other) {
    m_id = other.m_id;
    m_imagePath = other.m_imagePath;
    m_segmentationImagePath = other.m_segmentationImagePath;
    m_basePath = other.m_basePath;
    m_cameraMatrix = other.m_cameraMatrix;
}

QString Image::imagePath() const {
    return m_imagePath;
}

QString Image::absoluteImagePath() const {
    return QDir(m_basePath).filePath(m_imagePath);
}

QString Image::segmentationImagePath() const {
    return m_segmentationImagePath;
}

QString Image::absoluteSegmentationImagePath() const {
    // The segmentation image path is stored as absolute
    return m_segmentationImagePath;
}

QString Image::getBasePath() const {
    return m_basePath;
}

QMatrix3x3 Image::getCameraMatrix() const {
    return m_cameraMatrix;
}

bool Image::operator==(const Image &other) {
    // QString supports standard string comparison ==
    return m_basePath == other.m_basePath &&
            m_imagePath == other.m_imagePath &&
            m_segmentationImagePath == other.m_segmentationImagePath &&
            m_cameraMatrix == other.m_cameraMatrix;
}

Image& Image::operator=(const Image &other) {
    m_basePath = other.m_basePath;
    m_imagePath = other.m_imagePath;
    m_segmentationImagePath = other.m_segmentationImagePath;
    m_cameraMatrix = other.m_cameraMatrix;
    return *this;
}

float Image::farPlane() const {
    return m_farPlane;
}

QString Image::id() const
{
    return m_id;
}

float Image::nearPlane() const {
    return m_nearPlane;
}
