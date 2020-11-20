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
bool DisplayHelper::m_qtAwesomeInitialized = false;

QtAwesome *DisplayHelper::qtAwesome() {
    if (!DisplayHelper::m_qtAwesomeInitialized) {
        DisplayHelper::m_qtAwesome->initFontAwesome();
        DisplayHelper::m_qtAwesomeInitialized = true;
    }
    return DisplayHelper::m_qtAwesome;
}

void DisplayHelper::setIcon(QPushButton *button, fa::icon icon, int size) {
    QtAwesome *_qtAwesome = DisplayHelper::qtAwesome();
    button->setIcon(_qtAwesome->icon(icon));
    button->setFont(_qtAwesome->font(size));
}

QIcon DisplayHelper::warningIcon() {
    return getAwesomeIcon(fa::exclamationtriangle);
}

QIcon DisplayHelper::yesIcon() {
    return getAwesomeIcon(fa::checkcircle);
}

QIcon DisplayHelper::noIcon() {
    return getAwesomeIcon(fa::timescircle);
}

QIcon DisplayHelper::awesomeIconForButtonRole(QMessageBox::ButtonRole buttonRole) {
    switch (buttonRole) {
        case QMessageBox::AcceptRole:
            return yesIcon();
        case QMessageBox::RejectRole:
            return noIcon();
        case QMessageBox::DestructiveRole:
            return noIcon();
        case QMessageBox::ActionRole:
            return yesIcon();
        case QMessageBox::HelpRole:
        case QMessageBox::YesRole:
            return yesIcon();
        case QMessageBox::NoRole:
            return noIcon();
        case QMessageBox::ResetRole:
            return yesIcon();
        case QMessageBox::ApplyRole:
            return yesIcon();
        default:
            return QIcon();
    }
}

QIcon DisplayHelper::awesomeIconForMessageBoxIcon(QMessageBox::Icon icon) {
    switch (icon) {
        case QMessageBox::Information:
            return getAwesomeIcon(fa::infocircle);
        case QMessageBox::Warning:
            return warningIcon();
        case QMessageBox::Critical:
            return getAwesomeIcon(fa::timescircle);
        case QMessageBox::Question:
            return getAwesomeIcon(fa::questioncircle);
        default:
            return QIcon();
    }
}

QIcon DisplayHelper::getAwesomeIcon(fa::icon icon) {
    QtAwesome *_qtAwesome = DisplayHelper::qtAwesome();
    return _qtAwesome->icon(icon);
}

DisplayHelper::QMessageBoxPtr DisplayHelper::messageBox(QWidget* parent,
                                                        QMessageBox::Icon icon,
                                                        const QString &title, const QString &message,
                                                        const QString &buttonText, QMessageBox::ButtonRole buttonRole) {
    QMessageBoxPtr _messageBox(new QMessageBox(parent));
    _messageBox->setWindowTitle(title);
    _messageBox->setText(message);
    QPixmap iconPixmap = awesomeIconForMessageBoxIcon(icon).pixmap(QSize(50, 50));
    _messageBox->setIconPixmap(iconPixmap);
    QPushButton *button = _messageBox->addButton(buttonText, buttonRole);
    button->setIcon(awesomeIconForButtonRole(buttonRole));
    return _messageBox;
}

DisplayHelper::QMessageBoxPtr DisplayHelper::messageBox(QWidget* parent,
                                                        QMessageBox::Icon icon,
                                                        const QString &title, const QString &message,
                                                        const QString &button1Text, QMessageBox::ButtonRole button1Role,
                                                        const QString &button2Text, QMessageBox::ButtonRole button2Role) {
    QMessageBoxPtr _messageBox = messageBox(parent, icon, title, message, button1Text, button1Role);
    QPushButton *button = _messageBox->addButton(button2Text, button2Role);
    button->setIcon(awesomeIconForButtonRole(button2Role));
    return _messageBox;
}
