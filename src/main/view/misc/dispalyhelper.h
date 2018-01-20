#ifndef DISPALYHELPER_H
#define DISPALYHELPER_H

#include <QColor>

namespace DispalyHelper {
    /*!
     * \brief colorForCorrespondencePointIndex returns the color for index of a correspondence
     * point. A correspondence point is a clicked point on the displayed image or on the displayed
     * 3D model, when the user is about to create a new ObjectImageCorrespondence. The points are
     * to be visualized with a certain color, the first one e.g. in red, the second one in blue
     * etc. The colors can be retrieved here.
     * \param index the index of the correspondence point, e.g. 0 for the first click, 1 for the second
     * \return the color for the correspondence point
     */
    QColor colorForCorrespondencePointIndex(int index);

}

#endif // DISPALYHELPER_H
