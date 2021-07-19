#ifndef DISPALYHELPER_H
#define DISPALYHELPER_H

#include "QtAwesome.h"

#include <memory>
#include <QColor>
#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QSize>
#include <QIcon>
#include <QMessageBox>
#include <QScopedPointer>

class DisplayHelper {

public:

    typedef std::unique_ptr<QMessageBox> QMessageBoxPtr;

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
    static void setIcon(QPushButton *button, fa::icon icon, int size);
    static QIcon getAwesomeIcon(fa::icon icon);
    static QIcon warningIcon();
    static QIcon yesIcon();
    static QIcon noIcon();
    static QIcon awesomeIconForButtonRole(QMessageBox::ButtonRole buttonRole);
    static QIcon awesomeIconForMessageBoxIcon(QMessageBox::Icon icon);
    static QMessageBoxPtr messageBox(QWidget* parent,
                                     QMessageBox::Icon icon,
                                     const QString &title, const QString &message,
                                     const QString &buttonText, QMessageBox::ButtonRole buttonRole);
    static QMessageBoxPtr messageBox(QWidget* parent,
                                     QMessageBox::Icon icon,
                                     const QString &title, const QString &message,
                                     const QString &button1Text, QMessageBox::ButtonRole button1Role,
                                     const QString &button2Text, QMessageBox::ButtonRole button2Role);
    static int indexToMultisampleSamlpes(int index);

private:
    static QPointer<QtAwesome> m_qtAwesome;
    static bool m_qtAwesomeInitialized;

};

#endif // DISPALYHELPER_H
