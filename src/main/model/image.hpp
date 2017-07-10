#ifndef IMAGE_H
#define IMAGE_H

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

/*!
 * \brief The Image class holds the path to the actual image, as well as, if provided the path to the already segmented image.
 * The base path points to the folder where the image is located at or where the subfolders lie where the image is located
 * at. This means that the path to the image has to be relative to the base path.
 */
class Image {
private:
    const path imagePath;
    const path segmentationImagePath;
    const path basePath;

public:
    //! Constructor of class Image.
    /*!
      Using this constructor will store the image path and base path and set an empyt string for the segmentation image path.
      \param _url the path to the image relative to the base path
      \param _basePath the base path to the folder where this image is directly located or within a subfolder
    */
    Image(const path _path, path _basePath);

    //! Constructor of class Image.
    /*!
      Using this constructor will store the image path and the segmentation image path.
      \param _url the path to the image relative to the base path
      \param _basePath the base path to the folder where the image is either directly located or located within a subfolder
      \param _segmentationUrl the path to the segmentation image
    */
    Image(const path _path, path _basePath, const path _segmentationPath);

    //! Returns the path to the actual image relative to the base path.
    /*!
      \return the path to the actual image relative to the base path
    */
    const path getImagePath() const;

    /*!
     * \brief getAbsoluteImagePath Returns the absolute path to the image, i.e. the base path concatenated witht the image path
     * \return the full path to the image
     */
    const path getAbsoluteImagePath() const;

    //! Returns the path to the segmented image of this image.
    /*!
      \return the path of segmented image
    */
    const path getSegmentationImagePath() const;

    /*!
     * \brief getAbsoluteSegmentationImagePath Returns the absolute path to the segmented image, i.e. the base path concatenated
     * with the segmentation image path.
     * \return the absolute path to the segmented image
     */
    const path getAbsoluteSegmentationImagePath() const;

    /*!
     * \brief getBasePath Returns the path to the folder where the image either is located at directly or is
     * within a subfolder.
     * \return the path where the image is located direclty or within a subfolder
     */
    const path getBasePath() const;

};

#endif // IMAGE_H
