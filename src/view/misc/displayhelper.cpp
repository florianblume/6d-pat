#include "displayhelper.hpp"

#include <QPushButton>
#include <QLabel>
#include <QVariant>
#include <QDebug>

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

    QMap<Icon, QString> iconCodes = QMap<Icon, QString>(std::map<Icon, QString>{
                                                            {Icon::ZOOM_MINUS, "\uf010"},
                                                            {Icon::ZOOM_PLUS, "\uf00e"},
                                                            {Icon::TRANSPARENCY, "\uf042"},
                                                            {Icon::FOLDEROPEN, "\uf07c"},
                                                            {Icon::CHEVRONLEFT, "\uf053"},
                                                            {Icon::CHEVRONRIGHT, "\uf054"},
                                                            {Icon::TOGGLEOFF, "\uf205"},
                                                            {Icon::TOGGLEON, "\uf205"},
                                                            {Icon::ARROWS, "\uf0b2"},
                                                            {Icon::WRENCH, "\uf0ad"},
                                                            {Icon::PAINTBRUSH, "\uf126"},
                                                            {Icon::CODEFRORK, "\uf5a9"},
                                                            {Icon::REMOVE, "\uf2ed"}
                                                        });

    void setIcon(QWidget *widget, Icon icon, int size) {
        QFont font;
        font.setFamily("FontAwesome");
        font.setPixelSize(size);

        QString iconCode = iconCodes[icon];
        qDebug() << iconCode;

        if (dynamic_cast<QPushButton*>(widget) == Q_NULLPTR &&
                dynamic_cast<QLabel*>(widget) == Q_NULLPTR) {
            qWarning() << "Tried to set icon on widget which is neither button nor label";
        }

        widget->setProperty("font", font);
        widget->setProperty("text", iconCode);
    }

}
