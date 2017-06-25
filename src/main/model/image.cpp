#include "image.h"

Image::Image(string _url) : url(_url), segmentationUrl("") {
}

Image::Image(string _url, string _segmentationUrl) : url(_url), segmentationUrl(_segmentationUrl) {
}

string Image::getUrl() {
    return url;
}

string Image::getSegmentationUrl() {
    return segmentationUrl;
}
