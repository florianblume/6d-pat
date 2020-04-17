#include "displayhelper.hpp"

namespace  DisplayHelper {

    QColor colorForPosePointIndex(int index) {
        switch(index) {
            case 0: return Qt::red;
            case 1: return Qt::green;
            case 2: return Qt::blue;
            case 3: return Qt::yellow;
            case 4: return Qt::magenta;
            case 5: return Qt::cyan;
            case 6: return Qt::darkGreen;
            case 7: return Qt::darkBlue;
            case 8: return Qt::darkRed;
            case 9: return Qt::darkYellow;
            case 10: return Qt::darkCyan;
            case 11: return Qt::darkMagenta;
            default: return Qt::black;
        }
    }

}
