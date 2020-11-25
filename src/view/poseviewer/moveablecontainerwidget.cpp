#include "moveablecontainerwidget.hpp"

#include <QLayout>
#include <QBoxLayout>
#include <QDebug>

MoveableContainerWidget::MoveableContainerWidget(QWidget *parent)
    : QWidget(parent) {
    QBoxLayout *newLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    newLayout->setSpacing(0);
    setLayout(newLayout);
}

void MoveableContainerWidget::setContainedChild(QWidget *containedChild) {
    layout()->addWidget(containedChild);
    this->resize(containedChild->size());
}

void MoveableContainerWidget::onMouseMoveEvent(QMouseEvent *event) {
    qDebug() << "moved";
}
