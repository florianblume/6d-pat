#include "displayhelper.hpp"

#include <QPushButton>
#include <QLabel>
#include <QVariant>
#include <QDebug>
#include <QApplication>

QColor DisplayHelper::colorForPosePointIndex(int index) {
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

// Eager loading of QtAwesome, we're going to use it definitely
QPointer<QtAwesome> DisplayHelper::m_qtAwesome = new QtAwesome(qApp);

QtAwesome *DisplayHelper::qtAwesome() {
    return DisplayHelper::m_qtAwesome;
}

void DisplayHelper::setIcon(QWidget *widget, fa::icon icon, int size) {
    if (QPushButton *button = dynamic_cast<QPushButton*>(widget)) {
        button->setIcon(DisplayHelper::m_qtAwesome->icon(icon));
        //button->setFont(DisplayHelper::m_qtAwesome->font(size));
    } else if (QLabel *label = dynamic_cast<QLabel*>(widget)) {
        label->setText(QChar(icon));
        //label->setFont(DisplayHelper::m_qtAwesome->font(size));
    }
}
