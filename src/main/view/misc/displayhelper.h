#ifndef DISPALYHELPER_H
#define DISPALYHELPER_H

#include <QColor>

namespace DisplayHelper {
    /*!
     * \brief colorForCorrespondencePointIndex returns the color for index of a correspondence
     * point. A correspondence point is a clicked point on the displayed image or on the displayed
     * 3D model, when the user is about to create a new ObjectImageCorrespondence. The points are
     * to be visualized with a certain color, the first one e.g. in red, the second one in blue
     * etc. The colors can be retrieved here.
     * \param index the index of the correspondence point, e.g. 0 for the first click, 1 for the second
     * \return the color for the correspondence point
     */
    static QColor colorForCorrespondencePointIndex(int index) {
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

#endif // DISPALYHELPER_H
