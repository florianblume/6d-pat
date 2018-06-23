#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QMatrix3x3>

/*!
 * \brief The Image class holds the path to the actual image, as well as, if provided the path to the already segmented image.
 * The base path points to the folder where the image is located at or where the subfolders lie where the image is located
 * at. This means that the path to the image has to be relative to the base path.
 */
class Image {

public:

    Image();

    //! Constructor of class Image.
    /*!
      Using this constructor will store the image path and base path and set an empyt string for the segmentation image path.
      \param imagePath the path to the image relative to the base path
      \param basePath the base path to the folder where this image is directly located or within a subfolder
    */
    Image(const QString& imagePath, const QString& basePath, QMatrix3x3 cameraMatrix);

    //! Constructor of class Image.
    /*!
      Using this constructor will store the image path and the segmentation image path.
      \param imagePath the path to the image relative to the base path
      \param basePath the base path to the folder where the image is either directly located or located within a subfolder
      \param segmentationImagePath the path to the segmentation image
    */
    Image(const QString& imagePath, const QString& basePath,
          const QString& segmentationImagePath, QMatrix3x3 cameraMatrix);

    Image(const Image &other);

    //! Returns the path to the actual image relative to the base path.
    /*!
      \return the path to the actual image relative to the base path
    */
    QString getImagePath() const;

    /*!
     * \brief getAbsoluteImagePath Returns the absolute path to the image, i.e. the base path concatenated witht the image path
     * \return the full path to the image
     */
    QString getAbsoluteImagePath() const;

    //! Returns the path to the segmented image of this image.
    /*!
      \return the path of segmented image
    */
    QString getSegmentationImagePath() const;

    /*!
     * \brief getAbsoluteSegmentationImagePath Returns the absolute path to the segmented image, i.e. the base path concatenated
     * with the segmentation image path.
     * \return the absolute path to the segmented image
     */
    QString getAbsoluteSegmentationImagePath() const;

    /*!
     * \brief getBasePath Returns the path to the folder where the image either is located at directly or is
     * within a subfolder.
     * \return the path where the image is located direclty or within a subfolder
     */
    QString getBasePath() const;

    QMatrix3x3 getCameraMatrix() const;

    bool operator==(const Image &other);

    Image& operator=(const Image &other);

private:
    QString imagePath;
    QString segmentationImagePath;
    QString basePath;
    QMatrix3x3 cameraMatrix;

};

#endif // IMAGE_H
