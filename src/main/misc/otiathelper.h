#ifndef OTIATHELPER_H
#define OTIATHELPER_H

#include <string>

class OtiatHelper {
public:
    static int sign(int x) {
        return (x > 0) - (x < 0);
    }

    static std::string segmentationCodeForObjectModel(int red, int green, int blue) {
        return std::to_string(red) + "." + std::to_string(green) + "." + std::to_string(blue);
    }
};

#endif // OTIATHELPER_H
