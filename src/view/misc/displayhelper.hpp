#ifndef DISPALYHELPER_H
#define DISPALYHELPER_H

#include <QColor>
#include <QWidget>
#include <QMap>

namespace DisplayHelper {
    /*!
     * \brief colorForPosePointIndex returns the color for index of a pose
     * point. A pose point is a clicked point on the displayed image or on the displayed
     * 3D model, when the user is about to create a new ObjectImagePose. The points are
     * to be visualized with a certain color, the first one e.g. in red, the second one in blue
     * etc. The colors can be retrieved here.
     * \param index the index of the pose point, e.g. 0 for the first click, 1 for the second
     * \return the color for the pose point
     */
    QColor colorForPosePointIndex(int index);

    enum Icon {
        ZOOM_PLUS,
        ZOOM_MINUS,
        TRANSPARENCY,
        FOLDEROPEN,
        CHEVRONLEFT,
        CHEVRONRIGHT,
        TOGGLEOFF,
        TOGGLEON,
        ARROWS,
        WRENCH,
        PAINTBRUSH,
        CODEFRORK,
        REMOVE
    };

    void setIcon(QWidget *widget, Icon icon, int size);

}

#endif // DISPALYHELPER_H
