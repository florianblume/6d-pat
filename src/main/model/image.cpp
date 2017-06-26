#include "image.h"

Image::Image(const string _url) : url(_url), segmentationUrl("") {
}

Image::Image(const string _url, const string _segmentationUrl) : url(_url), segmentationUrl(_segmentationUrl) {
}

const string Image::getPath() const {
    return url;
}

const string Image::getSegmentationUrl() const {
    return segmentationUrl;
}
