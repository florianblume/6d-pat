#ifndef IMAGE_H
#define IMAGE_H

#include <string>

using namespace std;

//! This class holds the URL to the actual image, as well as, if provided the URL to the already segmented image.
class Image {
private:
    const string url;
    const string segmentationUrl;

public:
    //! Constructor of class Image.
    /*!
      Using this constructor will store the image URL and set an empyt string for the segmentation image URL.
      \param _url the URL to the image
    */
    Image(const string _url);

    //! Constructor of class Image.
    /*!
      Using this constructor will store the image URL and the segmentation image URL.
      \param _url the URL to the image
      \param _segmentationUrl the URL to the segmentation image
    */
    Image(const string _url, const string _segmentationUrl);

    //! Returns the URL stored by this Image.
    /*!
      \return the URL stored by this image
    */
    const string getPath() const;

    //! Returns the path of the segmentation image stored by this Image.
    /*!
      \return the path of the segmentation image stored by this image
    */
    const string getSegmentationUrl() const;
};

#endif // IMAGE_H
