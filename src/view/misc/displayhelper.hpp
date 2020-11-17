#ifndef DISPALYHELPER_H
#define DISPALYHELPER_H

#include "QtAwesome.h"

#include <QColor>
#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QSize>

class DisplayHelper {

public:
    /*!
     * \brief colorForPosePointIndex returns the color for index of a pose
     * point. A pose point is a clicked point on the displayed image or on the displayed
     * 3D model, when the user is about to create a new ObjectImagePose. The points are
     * to be visualized with a certain color, the first one e.g. in red, the second one in blue
     * etc. The colors can be retrieved here.
     * \param index the index of the pose point, e.g. 0 for the first click, 1 for the second
     * \return the color for the pose point
     */
    static QColor colorForPosePointIndex(int index);

    static QtAwesome *qtAwesome();
    static void setIcon(QWidget *widget, fa::icon icon, int size);

private:
    static QPointer<QtAwesome> m_qtAwesome;

};

#endif // DISPALYHELPER_H
